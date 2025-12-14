#include "LoginWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"
#include "Components/PanelWidget.h"
#include "Components/ComboBoxString.h"
#include "GameFramework/PlayerController.h"
#include "FibulaHUD.h"
#include "Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Config/ServerConfig.h"
#include "AuthTokenManager.h"
#include "Kismet/KismetSystemLibrary.h"

ULoginWidget::ULoginWidget(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
    bIsRequestInProgress = false;
}

void ULoginWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (LoginButton)
    {
        LoginButton->OnClicked.AddDynamic(this, &ULoginWidget::OnLoginButtonClicked);
    }

    if (RegisterButton)
    {
        RegisterButton->OnClicked.AddDynamic(this, &ULoginWidget::OnRegisterButtonClicked);
    }

    if (SwitchToLoginButton)
    {
        SwitchToLoginButton->OnClicked.AddDynamic(this, &ULoginWidget::OnSwitchToLoginClicked);
    }

    if (SwitchToRegisterButton)
    {
        SwitchToRegisterButton->OnClicked.AddDynamic(this, &ULoginWidget::OnSwitchToRegisterClicked);
    }

    if (LogoutButton)
    {
        LogoutButton->OnClicked.AddDynamic(this, &ULoginWidget::OnLogoutButtonClicked);
    }

    if (PlayButton)
    {
        PlayButton->OnClicked.AddDynamic(this, &ULoginWidget::OnPlayButtonClicked);
    }

    if (ExitGameButton)
    {
        ExitGameButton->OnClicked.AddDynamic(this, &ULoginWidget::OnExitGameButtonClicked);
    }

    
    if (UAuthTokenManager::HasToken())
    {
        ShowCharacterPanel();
        FetchCharacterList();
    }
    else
    {
        ShowLoginPanel();
    }

    if (CreateCharacterButton)
    {
        CreateCharacterButton->OnClicked.AddDynamic(this, &ULoginWidget::OnCreateCharacterButtonClicked);
    }

    if (BackToCharacterListButton)
    {
        BackToCharacterListButton->OnClicked.AddDynamic(this, &ULoginWidget::OnBackToCharacterListClicked);
    }

    
    if (VocationComboBox)
    {
        VocationComboBox->ClearOptions();
        VocationComboBox->AddOption(TEXT("Knight"));
        VocationComboBox->AddOption(TEXT("Paladin"));
        VocationComboBox->AddOption(TEXT("Sorcerer"));
        VocationComboBox->AddOption(TEXT("Druid"));
        VocationComboBox->SetSelectedOption(TEXT("Sorcerer"));
    }

    if (GameModeComboBox)
    {
        GameModeComboBox->ClearOptions();
        GameModeComboBox->AddOption(TEXT("Team Battle"));
        GameModeComboBox->AddOption(TEXT("Free-for-all"));
        GameModeComboBox->SetSelectedOption(TEXT("Free-for-all"));
    }

    if (CreateNewCharacterButton)
    {
        CreateNewCharacterButton->OnClicked.AddDynamic(this, &ULoginWidget::ShowCreateCharacterPanel);
    }

    if (CancelQueueButton)
    {
        CancelQueueButton->OnClicked.AddDynamic(this, &ULoginWidget::OnCancelQueueButtonClicked);
        CancelQueueButton->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void ULoginWidget::NativeDestruct()
{
    Super::NativeDestruct();

    
    if (UWorld *World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(LoginQueueTimerHandle);
        World->GetTimerManager().ClearTimer(ButtonReleaseTimerHandle);
    }
}

void ULoginWidget::OnLoginButtonClicked()
{
    FString ErrorMessage;
    if (!ValidateCredentials(ErrorMessage))
    {
        ShowError(ErrorMessage);
        return;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    FString URL = FString::Printf(TEXT("%s/api/login"), *ServerConfig::GetLoginServer());

    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    JsonObject->SetStringField(TEXT("email"), *EmailInputBox->GetText().ToString());
    JsonObject->SetStringField(TEXT("password"), *PasswordInputBox->GetText().ToString());

    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetContentAsString(JsonString);
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &ULoginWidget::OnLoginResponse);
    HttpRequest->ProcessRequest();
}

void ULoginWidget::OnLoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    if (!HandleServerResponse(Response, bSuccess, TEXT("Login failed")))
    {
        return;
    }

    const TSharedPtr<FJsonObject> JsonObject = GetJsonObject(Response);
    const TSharedPtr<FJsonObject> *DataObject;
    if (JsonObject->TryGetObjectField(TEXT("data"), DataObject))
    {
        FString Token = (*DataObject)->GetStringField("token");
        UAuthTokenManager::StoreToken(Token);
        FetchCharacterList();
    }
}

void ULoginWidget::FetchCharacterList()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    FString URL = FString::Printf(TEXT("%s/api/characters"), *ServerConfig::GetLoginServer());

    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb(TEXT("GET"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *UAuthTokenManager::GetToken()));
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &ULoginWidget::OnCharacterListResponse);
    HttpRequest->ProcessRequest();
}

void ULoginWidget::OnCharacterListResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    if (!bSuccess || !Response.IsValid())
    {
        ShowError(TEXT("Failed to fetch characters"));
        return;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        ShowError(TEXT("Invalid server response"));
        return;
    }

    if (CharacterComboBox)
    {
        CharacterComboBox->ClearOptions();
        const TArray<TSharedPtr<FJsonValue>> *CharactersArray;
        if (JsonObject->TryGetArrayField(TEXT("characters"), CharactersArray))
        {
            FString LastPlayedCharacter = UAuthTokenManager::GetLastPlayedCharacter();
            int32 LastPlayedIndex = -1;

            for (int32 i = 0; i < CharactersArray->Num(); ++i)
            {
                const TSharedPtr<FJsonObject> *CharacterObject;
                if ((*CharactersArray)[i]->TryGetObject(CharacterObject))
                {
                    FString Name = (*CharacterObject)->GetStringField(TEXT("name"));
                    FString Vocation = (*CharacterObject)->GetStringField(TEXT("vocation"));
                    FString DisplayText = FString::Printf(TEXT("%s (%s)"), *Name, *Vocation);

                    UE_LOG(LogTemp, Log, TEXT("Character Option: %s"), *DisplayText);
                    
                    CharacterComboBox->AddOption(DisplayText);

                    
                    if (!LastPlayedCharacter.IsEmpty() && Name.Equals(LastPlayedCharacter))
                    {
                        LastPlayedIndex = i;
                    }
                }
            }

            
            if (CharactersArray->Num() > 0)
            {
                if (LastPlayedIndex != -1)
                {
                    CharacterComboBox->SetSelectedIndex(LastPlayedIndex);
                }
                else
                {
                    CharacterComboBox->SetSelectedIndex(0);
                }
            }
        }
        ShowCharacterPanel();
    }
}

void ULoginWidget::OnLogoutButtonClicked()
{
    UAuthTokenManager::ClearToken();
    UAuthTokenManager::ClearLastPlayedCharacter();
    ShowLoginPanel();
    if (EmailInputBox)
        EmailInputBox->SetText(FText::GetEmpty());
    if (PasswordInputBox)
        PasswordInputBox->SetText(FText::GetEmpty());
}

void ULoginWidget::OnPlayButtonClicked()
{
    if (!CharacterComboBox)
        return;

    FString SelectedDisplayText = CharacterComboBox->GetSelectedOption();
    if (SelectedDisplayText.IsEmpty())
    {
        ShowError(TEXT("Please select a character"));
        return;
    }

    bIsRequestInProgress = true;
    if (PlayButton)
        PlayButton->SetIsEnabled(false);

    FString CharacterName;
    SelectedDisplayText.Split(TEXT(" ("), &CharacterName, nullptr);
    if (CharacterName.IsEmpty())
    {
        ShowError(TEXT("Invalid character selection"));
        return;
    }

    UAuthTokenManager::StoreLastPlayedCharacter(CharacterName);
    StartLoginQueue(CharacterName);
}

void ULoginWidget::StartLoginQueue(const FString &CharacterName)
{
    bIsInLoginQueue = true;
    QueuedCharacterName = CharacterName;

    
    if (CharacterPanel)
        CharacterPanel->SetVisibility(ESlateVisibility::Collapsed);
    if (QueuePanel)
        QueuePanel->SetVisibility(ESlateVisibility::Visible);
    if (CancelQueueButton)
        CancelQueueButton->SetVisibility(ESlateVisibility::Visible);
    SetQueueStatusText(TEXT("Connecting to game server..."));

    PollLoginQueue();
}

void ULoginWidget::SetQueueStatusText(const FString &Message)
{
    if (QueueStatusText)
        QueueStatusText->SetText(FText::FromString(Message));
}

void ULoginWidget::StopLoginQueue()
{
    bIsInLoginQueue = false;
    QueuedCharacterName.Empty();
    GetWorld()->GetTimerManager().ClearTimer(LoginQueueTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(ButtonReleaseTimerHandle);
    SetQueueStatusText(TEXT(""));

    if (QueuePanel)
        QueuePanel->SetVisibility(ESlateVisibility::Collapsed);
    if (CancelQueueButton)
        CancelQueueButton->SetVisibility(ESlateVisibility::Collapsed);
    if (CharacterPanel)
        CharacterPanel->SetVisibility(ESlateVisibility::Visible);
    if (PlayButton)
        PlayButton->SetIsEnabled(true);

    bIsRequestInProgress = false;
}

void ULoginWidget::OnCancelQueueButtonClicked()
{
    StopLoginQueue();
    ShowCharacterPanel();
}

void ULoginWidget::PollLoginQueue()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    FString EncodedCharacterName = FGenericPlatformHttp::UrlEncode(QueuedCharacterName);
    FString GameVersion = FGenericPlatformHttp::UrlEncode(ServerConfig::GAME_VERSION);

    
    FString GameMode = GameModeComboBox ? GameModeComboBox->GetSelectedOption() : TEXT("Team Battle");
    FString NormalizedGameMode = NormalizeGameMode(GameMode);
    FString EncodedGameMode = FGenericPlatformHttp::UrlEncode(NormalizedGameMode);

    FString URL = FString::Printf(TEXT("%s/api/server/recommend?version=%s&game_mode=%s&character=%s"),
                                  *ServerConfig::GetLoginServer(),
                                  *GameVersion,
                                  *EncodedGameMode,
                                  *EncodedCharacterName);

    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb(TEXT("GET"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *UAuthTokenManager::GetToken()));
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &ULoginWidget::OnServerRecommendationReceived);
    HttpRequest->ProcessRequest();
}

void ULoginWidget::ShowLoginFeedback(const FString &Message)
{
    if (ErrorText)
    {
        ErrorText->SetText(FText::FromString(Message));
    }
}

bool ULoginWidget::IsTransientError(const FString &ErrorCode)
{
    static const TSet<FString> TransientErrors = {
        TEXT("SERVERS_FULL"),
        TEXT("SERVER_BUSY"),
        TEXT("MAINTENANCE")};

    return TransientErrors.Contains(ErrorCode);
}

bool ULoginWidget::ValidateCredentials(FString &ErrorMessage)
{
    if (!EmailInputBox || !PasswordInputBox)
    {
        ErrorMessage = TEXT("UI components not initialized properly");
        return false;
    }

    FString Email = EmailInputBox->GetText().ToString();
    FString Password = PasswordInputBox->GetText().ToString();

    if (Email.IsEmpty() || Password.IsEmpty())
    {
        ErrorMessage = TEXT("Email and password are required");
        return false;
    }

    if (!Email.Contains(TEXT("@")))
    {
        ErrorMessage = TEXT("Invalid email format");
        return false;
    }

    if (Password.Len() < 6)
    {
        ErrorMessage = TEXT("Password must be at least 6 characters");
        return false;
    }

    return true;
}

void ULoginWidget::ShowError(const FString &ErrorMessage)
{
    if (ErrorText)
    {
        ErrorText->SetText(FText::FromString(ErrorMessage));
    }
}

void ULoginWidget::OnServerRecommendationReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    if (!bSuccess || !Response.IsValid())
    {
        if (bIsInLoginQueue)
        {
            ShowLoginFeedback(TEXT("Server unavailable, retrying..."));
            GetWorld()->GetTimerManager().SetTimer(LoginQueueTimerHandle, this, &ULoginWidget::PollLoginQueue, 5.0f, false);
            return;
        }
        ShowError(TEXT("Failed to get server recommendation"));
        StopLoginQueue();
        return;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        ShowError(TEXT("Invalid server response"));
        StopLoginQueue();
        return;
    }

    FString Status;
    if (JsonObject->TryGetStringField(TEXT("status"), Status) && Status == TEXT("error"))
    {
        FString ErrorCode, ErrorMessage;
        JsonObject->TryGetStringField(TEXT("code"), ErrorCode);
        JsonObject->TryGetStringField(TEXT("message"), ErrorMessage);

        if (IsTransientError(ErrorCode))
        {
            SetQueueStatusText(ErrorMessage);
            GetWorld()->GetTimerManager().SetTimer(LoginQueueTimerHandle, this, &ULoginWidget::PollLoginQueue, 5.0f, false);
            return;
        }

        ShowError(ErrorMessage);
        StopLoginQueue();
        return;
    }

    const TSharedPtr<FJsonObject> *ServerObject;
    if (!JsonObject->TryGetObjectField(TEXT("server"), ServerObject))
    {
        ShowError(TEXT("Invalid server data"));
        StopLoginQueue();
        return;
    }

    FString ServerAddress = (*ServerObject)->GetStringField(TEXT("address"));
    int32 Port = (*ServerObject)->GetIntegerField(TEXT("port"));

    if (APlayerController *PC = GetOwningPlayer())
    {
        FURL BaseURL;
        BaseURL.Host = ServerAddress;
        BaseURL.Port = Port;
        BaseURL.Map = TEXT("/Game/Maps/Ankrahmun");
        BaseURL.AddOption(*FString::Printf(TEXT("PlayerName=%s"), *QueuedCharacterName));
        BaseURL.AddOption(*FString::Printf(TEXT("Token=%s"), *UAuthTokenManager::GetToken()));

        PC->ClientTravel(BaseURL.ToString(), TRAVEL_Absolute);
    }
}

void ULoginWidget::OnRegisterButtonClicked()
{
    FString ErrorMessage;
    if (!ValidateRegistrationCredentials(ErrorMessage))
    {
        ShowError(ErrorMessage);
        return;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    FString URL = FString::Printf(TEXT("%s/api/register"), *ServerConfig::GetLoginServer());

    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    JsonObject->SetStringField(TEXT("email"), *RegisterEmailInputBox->GetText().ToString());
    JsonObject->SetStringField(TEXT("password"), *RegisterPasswordInputBox->GetText().ToString());

    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetContentAsString(JsonString);
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &ULoginWidget::OnRegisterResponse);
    HttpRequest->ProcessRequest();
}

bool ULoginWidget::ValidateRegistrationCredentials(FString &ErrorMessage)
{
    if (!RegisterEmailInputBox || !RegisterPasswordInputBox || !RegisterPasswordConfirmBox)
    {
        ErrorMessage = TEXT("UI components not initialized properly");
        return false;
    }

    FString Email = RegisterEmailInputBox->GetText().ToString();
    FString Password = RegisterPasswordInputBox->GetText().ToString();
    FString PasswordConfirm = RegisterPasswordConfirmBox->GetText().ToString();

    if (Email.IsEmpty() || Password.IsEmpty() || PasswordConfirm.IsEmpty())
    {
        ErrorMessage = TEXT("Email and password are required");
        return false;
    }

    if (!Email.Contains(TEXT("@")))
    {
        ErrorMessage = TEXT("Invalid email format");
        return false;
    }

    if (Password.Len() < 6)
    {
        ErrorMessage = TEXT("Password must be at least 6 characters");
        return false;
    }

    if (Password != PasswordConfirm)
    {
        ErrorMessage = TEXT("Passwords do not match");
        return false;
    }

    return true;
}

void ULoginWidget::OnRegisterResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    if (!HandleServerResponse(Response, bSuccess, TEXT("Registration failed")))
    {
        return;
    }

    const TSharedPtr<FJsonObject> JsonObject = GetJsonObject(Response);
    const TSharedPtr<FJsonObject> *DataObject;
    if (JsonObject->TryGetObjectField(TEXT("data"), DataObject))
    {
        FString Token = (*DataObject)->GetStringField("token");
        UAuthTokenManager::StoreToken(Token);
        FetchCharacterList();
    }
}

void ULoginWidget::OnSwitchToLoginClicked()
{
    ShowLoginPanel();
}

void ULoginWidget::OnSwitchToRegisterClicked()
{
    ShowRegisterPanel();
}

void ULoginWidget::ShowLoginPanel()
{
    if (LoginPanel)
        LoginPanel->SetVisibility(ESlateVisibility::Visible);
    if (RegisterPanel)
        RegisterPanel->SetVisibility(ESlateVisibility::Collapsed);
    if (CharacterPanel)
        CharacterPanel->SetVisibility(ESlateVisibility::Collapsed);
    if (ErrorText)
        ErrorText->SetText(FText::GetEmpty());
}

void ULoginWidget::ShowRegisterPanel()
{
    if (LoginPanel)
        LoginPanel->SetVisibility(ESlateVisibility::Collapsed);
    if (RegisterPanel)
        RegisterPanel->SetVisibility(ESlateVisibility::Visible);
    if (CharacterPanel)
        CharacterPanel->SetVisibility(ESlateVisibility::Collapsed);
    if (ErrorText)
        ErrorText->SetText(FText::GetEmpty());
}

void ULoginWidget::ShowCharacterPanel()
{
    if (LoginPanel)
        LoginPanel->SetVisibility(ESlateVisibility::Collapsed);
    if (RegisterPanel)
        RegisterPanel->SetVisibility(ESlateVisibility::Collapsed);
    if (CharacterPanel)
        CharacterPanel->SetVisibility(ESlateVisibility::Visible);
    if (CreateCharacterPanel)
        CreateCharacterPanel->SetVisibility(ESlateVisibility::Collapsed);
    if (ErrorText)
        ErrorText->SetText(FText::GetEmpty());
}

void ULoginWidget::OnCreateCharacterButtonClicked()
{
    FString ErrorMessage;
    if (!ValidateCharacterCreation(ErrorMessage))
    {
        ShowError(ErrorMessage);
        return;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    FString URL = FString::Printf(TEXT("%s/api/characters"), *ServerConfig::GetLoginServer());

    
    TSharedPtr<FJsonObject> CharacterObject = MakeShared<FJsonObject>();
    CharacterObject->SetStringField(TEXT("name"), *CharacterNameInputBox->GetText().ToString());
    CharacterObject->SetStringField(TEXT("vocation"), *VocationComboBox->GetSelectedOption());

    
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    JsonObject->SetObjectField(TEXT("character"), CharacterObject);

    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *UAuthTokenManager::GetToken()));
    HttpRequest->SetContentAsString(JsonString);
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &ULoginWidget::OnCreateCharacterResponse);
    HttpRequest->ProcessRequest();
}

bool ULoginWidget::ValidateCharacterCreation(FString &ErrorMessage)
{
    if (!CharacterNameInputBox || !VocationComboBox)
    {
        ErrorMessage = TEXT("UI components not initialized properly");
        return false;
    }

    FString CharacterName = CharacterNameInputBox->GetText().ToString();

    if (CharacterName.IsEmpty())
    {
        ErrorMessage = TEXT("Character name is required");
        return false;
    }

    if (CharacterName.Len() < 2 || CharacterName.Len() > 20)
    {
        ErrorMessage = TEXT("Character name must be between 2 and 20 characters");
        return false;
    }

    
    for (TCHAR Character : CharacterName)
    {
        if (!FChar::IsAlpha(Character) && !FChar::IsWhitespace(Character))
        {
            ErrorMessage = TEXT("Character name can only contain letters and spaces");
            return false;
        }
    }

    return true;
}

void ULoginWidget::OnCreateCharacterResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    if (!HandleServerResponse(Response, bSuccess, TEXT("Failed to create character")))
    {
        return;
    }

    if (CharacterNameInputBox)
    {
        CharacterNameInputBox->SetText(FText::GetEmpty());
    }

    ShowCharacterPanel();
    FetchCharacterList();
}

void ULoginWidget::OnBackToCharacterListClicked()
{
    ShowCharacterPanel();
}

void ULoginWidget::ShowCreateCharacterPanel()
{
    if (LoginPanel)
        LoginPanel->SetVisibility(ESlateVisibility::Collapsed);
    if (RegisterPanel)
        RegisterPanel->SetVisibility(ESlateVisibility::Collapsed);
    if (CharacterPanel)
        CharacterPanel->SetVisibility(ESlateVisibility::Collapsed);
    if (CreateCharacterPanel)
        CreateCharacterPanel->SetVisibility(ESlateVisibility::Visible);
    if (ErrorText)
        ErrorText->SetText(FText::GetEmpty());
    if (CharacterNameInputBox)
        CharacterNameInputBox->SetText(FText::GetEmpty());
    if (VocationComboBox)
        VocationComboBox->SetSelectedOption(TEXT("Knight"));
}

bool ULoginWidget::HandleServerResponse(FHttpResponsePtr Response, bool bSuccess, const FString &FailureMessage)
{
    if (!bSuccess || !Response.IsValid())
    {
        ShowError(FailureMessage);
        return false;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        ShowError(TEXT("Invalid server response"));
        return false;
    }

    FString Status = JsonObject->GetStringField("status");
    if (Status != TEXT("success"))
    {
        const TSharedPtr<FJsonObject> *ErrorsObject;
        if (JsonObject->TryGetObjectField(TEXT("errors"), ErrorsObject))
        {
            
            for (const auto &Pair : (*ErrorsObject)->Values)
            {
                const TArray<TSharedPtr<FJsonValue>> *FieldErrors;
                if ((*ErrorsObject)->TryGetArrayField(Pair.Key, FieldErrors) && FieldErrors->Num() > 0)
                {
                    ShowError((*FieldErrors)[0]->AsString());
                    return false;
                }
            }

            
            FString Message;
            if ((*ErrorsObject)->TryGetStringField(TEXT("message"), Message))
            {
                ShowError(Message);
                return false;
            }
        }

        ShowError(FailureMessage);
        return false;
    }

    return true;
}

TSharedPtr<FJsonObject> ULoginWidget::GetJsonObject(FHttpResponsePtr Response)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
    FJsonSerializer::Deserialize(Reader, JsonObject);
    return JsonObject;
}

void ULoginWidget::OnExitGameButtonClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}

FString ULoginWidget::NormalizeGameMode(const FString &DisplayGameMode)
{
    if (DisplayGameMode.Equals(TEXT("Free-for-all"), ESearchCase::IgnoreCase))
        return TEXT("FreeForAll");
    if (DisplayGameMode.Equals(TEXT("Team Battle"), ESearchCase::IgnoreCase))
        return TEXT("TeamBattle");

    
    return TEXT("TeamBattle");
}
