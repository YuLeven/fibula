#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterEnums.h"
#include "Http.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Config/ServerConfig.h"
#include "LoginWidget.generated.h"

USTRUCT()
struct FCharacterInfo
{
    GENERATED_BODY()

    UPROPERTY()
    FString Name;

    UPROPERTY()
    FString Vocation;
};

UCLASS()
class FIBULA_API ULoginWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    ULoginWidget(const FObjectInitializer &ObjectInitializer);

protected:
    
    UPROPERTY(meta = (BindWidget))
    class UPanelWidget *LoginPanel;

    
    UPROPERTY(meta = (BindWidget))
    class UPanelWidget *RegisterPanel;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox *EmailInputBox;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox *PasswordInputBox;

    UPROPERTY(meta = (BindWidget))
    class UButton *LoginButton;

    UPROPERTY(meta = (BindWidget))
    class UButton *RegisterButton;

    UPROPERTY(meta = (BindWidget))
    class UButton *SwitchToLoginButton;

    UPROPERTY(meta = (BindWidget))
    class UButton *SwitchToRegisterButton;

    
    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox *RegisterEmailInputBox;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox *RegisterPasswordInputBox;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox *RegisterPasswordConfirmBox;

    
    UPROPERTY(meta = (BindWidget))
    class UPanelWidget *CharacterPanel;

    UPROPERTY(meta = (BindWidget))
    class UComboBoxString *CharacterComboBox;

    UPROPERTY(meta = (BindWidget))
    class UButton *LogoutButton;

    UPROPERTY(meta = (BindWidget))
    class UButton *PlayButton;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock *ErrorText;

    
    UPROPERTY(meta = (BindWidget))
    class UPanelWidget *CreateCharacterPanel;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox *CharacterNameInputBox;

    UPROPERTY(meta = (BindWidget))
    class UComboBoxString *VocationComboBox;

    UPROPERTY(meta = (BindWidget))
    class UButton *CreateCharacterButton;

    UPROPERTY(meta = (BindWidget))
    class UButton *BackToCharacterListButton;

    UPROPERTY(meta = (BindWidget))
    class UButton *CreateNewCharacterButton;

    UPROPERTY(meta = (BindWidget))
    class UButton *ExitGameButton;

    UPROPERTY(meta = (BindWidget))
    class UButton *CancelQueueButton;

    UPROPERTY(meta = (BindWidget))
    class UPanelWidget *QueuePanel;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock *QueueStatusText;

    UPROPERTY(meta = (BindWidget))
    class UComboBoxString *GameModeComboBox;

private:
    TArray<FCharacterInfo> Characters;
    bool bIsRequestInProgress;
    bool bIsInLoginQueue;
    FTimerHandle LoginQueueTimerHandle;
    FTimerHandle ButtonReleaseTimerHandle;
    FString QueuedCharacterName;

    UFUNCTION()
    void OnLoginButtonClicked();

    UFUNCTION()
    void OnLogoutButtonClicked();

    UFUNCTION()
    void OnPlayButtonClicked();

    UFUNCTION()
    void OnRegisterButtonClicked();

    UFUNCTION()
    void OnSwitchToLoginClicked();

    UFUNCTION()
    void OnSwitchToRegisterClicked();

    UFUNCTION()
    void OnExitGameButtonClicked();

    void ShowRegisterPanel();
    void OnRegisterResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);

    void ShowLoginPanel();
    void ShowCharacterPanel();
    bool ValidateCredentials(FString &ErrorMessage);
    void ShowError(const FString &ErrorMessage);
    void FetchCharacterList();
    void SetQueueStatusText(const FString &Message);

    void OnLoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
    void OnCharacterListResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
    void OnServerRecommendationReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);

    bool ValidateRegistrationCredentials(FString &ErrorMessage);

    UFUNCTION()
    void OnCreateCharacterButtonClicked();

    UFUNCTION()
    void OnBackToCharacterListClicked();

    UFUNCTION()
    void ShowCreateCharacterPanel();
    bool ValidateCharacterCreation(FString &ErrorMessage);
    void OnCreateCharacterResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);

    bool HandleServerResponse(FHttpResponsePtr Response, bool bSuccess, const FString &FailureMessage);

    TSharedPtr<FJsonObject> GetJsonObject(FHttpResponsePtr Response);

    void StartLoginQueue(const FString &CharacterName);
    void StopLoginQueue();
    void PollLoginQueue();
    void ShowLoginFeedback(const FString &Message);
    bool IsTransientError(const FString &ErrorMessage);

    UFUNCTION()
    void OnCancelQueueButtonClicked();

    FString NormalizeGameMode(const FString &DisplayGameMode);
};