// Copyright 2019 - Roberto De Ioris

#include "LuaBlueprintFunctionLibrary.h"
#include "LuaComponent.h"
#include "LuaMachine.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpResponse.h"
#include "Runtime/Core/Public/Math/BigInt.h"
#include "Runtime/Core/Public/Misc/Base64.h"
#include "Runtime/Core/Public/Misc/SecureHash.h"

FLuaValue ULuaBlueprintFunctionLibrary::LuaCreateNil()
{
	return FLuaValue();
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaCreateString(FString String)
{
	return FLuaValue(String);
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaCreateNumber(float Value)
{
	return FLuaValue(Value);
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaCreateInteger(int32 Value)
{
	return FLuaValue(Value);
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaCreateBool(bool bInBool)
{
	return FLuaValue(bInBool);
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaCreateObject(UObject* InObject)
{
	return FLuaValue(InObject);
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaCreateTable(UObject* WorldContextObject, TSubclassOf<ULuaState> State)
{
	FLuaValue LuaValue;
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return LuaValue;

	return L->CreateLuaTable();
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaCreateObjectInState(UObject* WorldContextObject, TSubclassOf<ULuaState> State, UObject* InObject)
{
	FLuaValue LuaValue;
	if (!InObject)
		return LuaValue;
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return LuaValue;

	LuaValue = FLuaValue(InObject);
	LuaValue.LuaState = L;
	return LuaValue;
}

FString ULuaBlueprintFunctionLibrary::Conv_LuaValueToString(FLuaValue Value)
{
	return Value.ToString();
}

FName ULuaBlueprintFunctionLibrary::Conv_LuaValueToName(FLuaValue Value)
{
	return FName(*Value.ToString());
}

FText ULuaBlueprintFunctionLibrary::Conv_LuaValueToText(FLuaValue Value)
{
	return FText::FromString(Value.ToString());
}

UObject* ULuaBlueprintFunctionLibrary::Conv_LuaValueToObject(FLuaValue Value)
{
	if (Value.Type == ELuaValueType::UObject)
	{
		return Value.Object;
	}
	return nullptr;
}

UClass* ULuaBlueprintFunctionLibrary::Conv_LuaValueToClass(FLuaValue Value)
{
	if (Value.Type == ELuaValueType::UObject)
	{
		UClass* Class = Cast<UClass>(Value.Object);
		if (Class)
			return Class;
		UBlueprint* Blueprint = Cast<UBlueprint>(Value.Object);
		if (Blueprint)
			return Blueprint->GeneratedClass;
	}
	return nullptr;
}

FLuaValue ULuaBlueprintFunctionLibrary::Conv_ObjectToLuaValue(UObject* Object)
{
	return FLuaValue(Object);
}


FLuaValue ULuaBlueprintFunctionLibrary::Conv_FloatToLuaValue(float Value)
{
	return FLuaValue(Value);
}

FLuaValue ULuaBlueprintFunctionLibrary::Conv_BoolToLuaValue(bool Value)
{
	return FLuaValue(Value);
}

int32 ULuaBlueprintFunctionLibrary::Conv_LuaValueToInt(FLuaValue Value)
{
	return Value.ToInteger();
}

float ULuaBlueprintFunctionLibrary::Conv_LuaValueToFloat(FLuaValue Value)
{
	return Value.ToFloat();
}

bool ULuaBlueprintFunctionLibrary::Conv_LuaValueToBool(FLuaValue Value)
{
	return Value.ToBool();
}

FLuaValue ULuaBlueprintFunctionLibrary::Conv_IntToLuaValue(int32 Value)
{
	return FLuaValue(Value);
}

FLuaValue ULuaBlueprintFunctionLibrary::Conv_StringToLuaValue(FString Value)
{
	return FLuaValue(Value);
}

FLuaValue ULuaBlueprintFunctionLibrary::Conv_TextToLuaValue(FText Value)
{
	return FLuaValue(Value.ToString());
}

FLuaValue ULuaBlueprintFunctionLibrary::Conv_NameToLuaValue(FName Value)
{
	return FLuaValue(Value.ToString());
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaGetGlobal(UObject* WorldContextObject, TSubclassOf<ULuaState> State, FString Name)
{
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return FLuaValue();

	uint32 ItemsToPop = L->GetFieldFromTree(Name);
	FLuaValue ReturnValue = L->ToLuaValue(-1);
	L->Pop(ItemsToPop);
	return ReturnValue;
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaRunFile(UObject* WorldContextObject, TSubclassOf<ULuaState> State, FString Filename, bool bIgnoreNonExistent)
{
	FLuaValue ReturnValue;

	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return ReturnValue;

	if (!L->RunFile(Filename, bIgnoreNonExistent, 1))
	{
		if (L->bLogError)
			L->LogError(L->LastError);
		L->ReceiveLuaError(L->LastError);
	}
	else
	{
		ReturnValue = L->ToLuaValue(-1);
	}

	L->Pop();
	return ReturnValue;
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaRunString(UObject* WorldContextObject, TSubclassOf<ULuaState> State, FString CodeString)
{
	FLuaValue ReturnValue;

	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return ReturnValue;

	if (!L->RunCode(CodeString, CodeString, 1))
	{
		if (L->bLogError)
			L->LogError(L->LastError);
		L->ReceiveLuaError(L->LastError);
	}
	else
	{
		ReturnValue = L->ToLuaValue(-1);
	}
	L->Pop();
	return ReturnValue;
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaRunCodeAsset(UObject* WorldContextObject, TSubclassOf<ULuaState> State, ULuaCode* CodeAsset)
{
	FLuaValue ReturnValue;

	if (!CodeAsset)
		return ReturnValue;

	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return ReturnValue;

	if (!L->RunCodeAsset(CodeAsset, 1))
	{
		if (L->bLogError)
			L->LogError(L->LastError);
		L->ReceiveLuaError(L->LastError);
	}
	else {
		ReturnValue = L->ToLuaValue(-1);
	}
	L->Pop();
	return ReturnValue;
}

void ULuaBlueprintFunctionLibrary::LuaRunURL(UObject* WorldContextObject, TSubclassOf<ULuaState> State, FString URL, TMap<FString, FString> Headers, FString SecurityHeader, FString SignaturePublicExponent, FString SignatureModulus, FLuaHttpSuccess Completed)
{

	// Security CHECK
	if (!SecurityHeader.IsEmpty() || !SignaturePublicExponent.IsEmpty() || !SignatureModulus.IsEmpty())
	{
		if (SecurityHeader.IsEmpty() || SignaturePublicExponent.IsEmpty() || SignatureModulus.IsEmpty())
		{
			UE_LOG(LogLuaMachine, Error, TEXT("For secure LuaRunURL() you need to specify the Security HTTP Header, the Signature Public Exponent and the Signature Modulus"));
			return;
		}
	}
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(URL);
	for (TPair<FString, FString> Header : Headers)
	{
		HttpRequest->AppendToHeader(Header.Key, Header.Value);
	}
	HttpRequest->OnProcessRequestComplete().BindStatic(&ULuaBlueprintFunctionLibrary::HttpRequestDone, State, TWeakObjectPtr<UWorld>(WorldContextObject->GetWorld()), SecurityHeader, SignaturePublicExponent, SignatureModulus, Completed);
	HttpRequest->ProcessRequest();
}

void ULuaBlueprintFunctionLibrary::HttpRequestDone(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, TSubclassOf<ULuaState> LuaState, TWeakObjectPtr<UWorld> World, FString SecurityHeader, FString SignaturePublicExponent, FString SignatureModulus, FLuaHttpSuccess Completed)
{
	FLuaValue ReturnValue;
	int32 StatusCode = -1;

	if (!bWasSuccessful)
	{
		UE_LOG(LogLuaMachine, Error, TEXT("HTTP session failed for \"%s %s\""), *Request->GetVerb(), *Request->GetURL());
	}

	else if (!World.IsValid())
	{
		UE_LOG(LogLuaMachine, Error, TEXT("Unable to access LuaState as the World object is no more available (\"%s %s\")"), *Request->GetVerb(), *Request->GetURL());
	}
	else
	{
		// Check signature
		if (!SecurityHeader.IsEmpty())
		{
			// check code size
			if (Response->GetContentLength() <= 0)
			{
				UE_LOG(LogLuaMachine, Error, TEXT("[Security] Invalid Content Size"));
				Completed.ExecuteIfBound(ReturnValue, bWasSuccessful, StatusCode);
				return;
			}
			FString EncryptesSignatureBase64 = Response->GetHeader(SecurityHeader);
			if (EncryptesSignatureBase64.IsEmpty())
			{
				UE_LOG(LogLuaMachine, Error, TEXT("[Security] Invalid Security HTTP Header"));
				Completed.ExecuteIfBound(ReturnValue, bWasSuccessful, StatusCode);
				return;
			}

			const uint32 KeySize = 64;

			TArray<uint8> Signature;
			FBase64::Decode(EncryptesSignatureBase64, Signature);

			if (Signature.Num() != KeySize)
			{
				UE_LOG(LogLuaMachine, Error, TEXT("[Security] Invalid Signature"));
				Completed.ExecuteIfBound(ReturnValue, bWasSuccessful, StatusCode);
				return;
			}
			TEncryptionInt SignatureValue = TEncryptionInt((uint32 *)&Signature[0]);

			TArray<uint8> PublicExponent;
			FBase64::Decode(SignaturePublicExponent, PublicExponent);
			if (PublicExponent.Num() != KeySize)
			{
				UE_LOG(LogLuaMachine, Error, TEXT("[Security] Invalid Signature Public Exponent"));
				Completed.ExecuteIfBound(ReturnValue, bWasSuccessful, StatusCode);
				return;
			}

			TArray<uint8> Modulus;
			FBase64::Decode(SignatureModulus, Modulus);
			if (Modulus.Num() != KeySize)
			{
				UE_LOG(LogLuaMachine, Error, TEXT("[Security] Invalid Signature Modulus"));
				Completed.ExecuteIfBound(ReturnValue, bWasSuccessful, StatusCode);
				return;
			}

			TEncryptionInt PublicKey = TEncryptionInt((uint32 *)&PublicExponent[0]);
			TEncryptionInt ModulusValue = TEncryptionInt((uint32 *)&Modulus[0]);

			TEncryptionInt ShaHash;
			FSHA1::HashBuffer(Response->GetContent().GetData(), Response->GetContentLength(), (uint8*)&ShaHash);

			TEncryptionInt DecryptedSignature = FEncryption::ModularPow(SignatureValue, PublicKey, ModulusValue);
			if (DecryptedSignature != ShaHash)
			{
				UE_LOG(LogLuaMachine, Error, TEXT("[Security] Signature check failed"));
				Completed.ExecuteIfBound(ReturnValue, bWasSuccessful, StatusCode);
				return;
			}
		}

		ReturnValue = LuaRunString(World.Get(), LuaState, Response->GetContentAsString());
		StatusCode = Response->GetResponseCode();
	}

	Completed.ExecuteIfBound(ReturnValue, bWasSuccessful, StatusCode);
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaTableGetField(FLuaValue Table, FString Key)
{
	FLuaValue ReturnValue;
	if (Table.Type != ELuaValueType::Table)
		return ReturnValue;

	ULuaState* L = Table.LuaState;
	if (!L)
		return ReturnValue;

	return Table.GetField(Key);
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaComponentGetField(FLuaValue LuaComponent, FString Key)
{
	FLuaValue ReturnValue;
	if (LuaComponent.Type != ELuaValueType::UObject)
		return ReturnValue;

	ULuaState* L = LuaComponent.LuaState;
	if (!L)
		return ReturnValue;

	ULuaComponent* Component = Cast<ULuaComponent>(LuaComponent.Object);

	FLuaValue* LuaValue = Component->Table.Find(Key);
	if (LuaValue)
	{
		return *LuaValue;
	}

	return ReturnValue;
}

bool ULuaBlueprintFunctionLibrary::LuaValueIsNil(FLuaValue Value)
{
	return Value.Type == ELuaValueType::Nil;
}

bool ULuaBlueprintFunctionLibrary::LuaValueIsOwned(FLuaValue Value)
{
	return Value.LuaState != nullptr;
}

TSubclassOf<ULuaState> ULuaBlueprintFunctionLibrary::LuaValueGetOwner(FLuaValue Value)
{
	if (!Value.LuaState)
		return nullptr;
	return Value.LuaState->GetClass();
}

bool ULuaBlueprintFunctionLibrary::LuaValueIsNotNil(FLuaValue Value)
{
	return Value.Type != ELuaValueType::Nil;
}

bool ULuaBlueprintFunctionLibrary::LuaValueIsTable(FLuaValue Value)
{
	return Value.Type == ELuaValueType::Table;
}

bool ULuaBlueprintFunctionLibrary::LuaValueIsBoolean(FLuaValue Value)
{
	return Value.Type == ELuaValueType::Bool;
}

bool ULuaBlueprintFunctionLibrary::LuaValueIsThread(FLuaValue Value)
{
	return Value.Type == ELuaValueType::Thread;
}

bool ULuaBlueprintFunctionLibrary::LuaValueIsFunction(FLuaValue Value)
{
	return Value.Type == ELuaValueType::Function;
}

bool ULuaBlueprintFunctionLibrary::LuaValueIsNumber(FLuaValue Value)
{
	return Value.Type == ELuaValueType::Number;
}

bool ULuaBlueprintFunctionLibrary::LuaValueIsInteger(FLuaValue Value)
{
	return Value.Type == ELuaValueType::Integer;
}

bool ULuaBlueprintFunctionLibrary::LuaValueIsString(FLuaValue Value)
{
	return Value.Type == ELuaValueType::String;
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaTableGetByIndex(FLuaValue Table, int32 Index)
{
	if (Table.Type != ELuaValueType::Table)
		return FLuaValue();

	ULuaState* L = Table.LuaState;
	if (!L)
		return FLuaValue();

	return Table.GetFieldByIndex(Index);
}

FLuaValue ULuaBlueprintFunctionLibrary::AssignLuaValueToLuaState(UObject* WorldContextObject, FLuaValue Value, TSubclassOf<ULuaState> State)
{
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return Value;
	Value.LuaState = L;
	return Value;
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaTableSetByIndex(FLuaValue Table, int32 Index, FLuaValue Value)
{
	if (Table.Type != ELuaValueType::Table)
		return FLuaValue();

	ULuaState* L = Table.LuaState;
	if (!L)
		return FLuaValue();

	return Table.SetFieldByIndex(Index, Value);
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaTableSetField(FLuaValue Table, FString Key, FLuaValue Value)
{
	FLuaValue ReturnValue;
	if (Table.Type != ELuaValueType::Table)
		return ReturnValue;

	ULuaState* L = Table.LuaState;
	if (!L)
		return ReturnValue;

	return Table.SetField(Key, Value);
}

FLuaValue ULuaBlueprintFunctionLibrary::GetLuaComponentAsLuaValue(AActor* Actor)
{
	if (!Actor)
		return FLuaValue();

	return FLuaValue(Actor->GetComponentByClass(ULuaComponent::StaticClass()));
}

FLuaValue ULuaBlueprintFunctionLibrary::GetLuaComponentByStateAsLuaValue(AActor* Actor, TSubclassOf<ULuaState> State)
{
	if (!Actor)
		return FLuaValue();

	TArray<UActorComponent *> Components = Actor->GetComponentsByClass(ULuaComponent::StaticClass());
	for (UActorComponent* Component : Components)
	{
		ULuaComponent* LuaComponent = Cast<ULuaComponent>(Component);
		if (LuaComponent)
		{
			if (LuaComponent->LuaState == State)
			{
				return FLuaValue(LuaComponent);
			}
		}
	}

	return FLuaValue();
}

FLuaValue ULuaBlueprintFunctionLibrary::GetLuaComponentByNameAsLuaValue(AActor* Actor, FString Name)
{
	if (!Actor)
		return FLuaValue();

	TArray<UActorComponent *> Components = Actor->GetComponentsByClass(ULuaComponent::StaticClass());
	for (UActorComponent* Component : Components)
	{
		ULuaComponent* LuaComponent = Cast<ULuaComponent>(Component);
		if (LuaComponent)
		{
			if (LuaComponent->GetName() == Name)
			{
				return FLuaValue(LuaComponent);
			}
		}
	}

	return FLuaValue();
}

FLuaValue ULuaBlueprintFunctionLibrary::GetLuaComponentByStateAndNameAsLuaValue(AActor* Actor, TSubclassOf<ULuaState> State, FString Name)
{
	if (!Actor)
		return FLuaValue();

	TArray<UActorComponent *> Components = Actor->GetComponentsByClass(ULuaComponent::StaticClass());
	for (UActorComponent* Component : Components)
	{
		ULuaComponent* LuaComponent = Cast<ULuaComponent>(Component);
		if (LuaComponent)
		{
			if (LuaComponent->LuaState == State && LuaComponent->GetName() == Name)
			{
				return FLuaValue(LuaComponent);
			}
		}
	}

	return FLuaValue();
}

int32 ULuaBlueprintFunctionLibrary::LuaGetTop(UObject* WorldContextObject, TSubclassOf<ULuaState> State)
{
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return MIN_int32;
	return L->GetTop();
}

void ULuaBlueprintFunctionLibrary::LuaSetGlobal(UObject* WorldContextObject, TSubclassOf<ULuaState> State, FString Name, FLuaValue Value)
{
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return;
	L->SetFieldFromTree(Name, Value);
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaGlobalCall(UObject* WorldContextObject, TSubclassOf<ULuaState> State, FString Name, TArray<FLuaValue> Args)
{
	FLuaValue ReturnValue;
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return ReturnValue;

	int32 ItemsToPop = L->GetFieldFromTree(Name);

	int NArgs = 0;
	for (FLuaValue& Arg : Args)
	{
		L->FromLuaValue(Arg);
		NArgs++;
	}

	L->PCall(NArgs, ReturnValue);

	// we have the return value and the function has been removed, so we do not need to change ItemsToPop
	L->Pop(ItemsToPop);

	return ReturnValue;
}

TArray<FLuaValue> ULuaBlueprintFunctionLibrary::LuaGlobalCallMulti(UObject* WorldContextObject, TSubclassOf<ULuaState> State, FString Name, TArray<FLuaValue> Args)
{
	TArray<FLuaValue> ReturnValue;
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return ReturnValue;

	int32 ItemsToPop = L->GetFieldFromTree(Name);

	int32 StackTop = L->GetTop();

	int NArgs = 0;
	for (FLuaValue& Arg : Args)
	{
		L->FromLuaValue(Arg);
		NArgs++;
	}

	FLuaValue LastReturnValue;
	if (L->PCall(NArgs, LastReturnValue, LUA_MULTRET))
	{

		int32 NumOfReturnValues = (L->GetTop() - StackTop) + 1;
		if (NumOfReturnValues > 0)
		{
			for (int32 i = -1; i >= -(NumOfReturnValues); i--)
			{
				ReturnValue.Insert(L->ToLuaValue(i), 0);
			}
			L->Pop(NumOfReturnValues - 1);
		}

	}

	// we have the return value and the function has been removed, so we do not need to change ItemsToPop
	L->Pop(ItemsToPop);

	return ReturnValue;
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaGlobalCallValue(UObject* WorldContextObject, TSubclassOf<ULuaState> State, FLuaValue Value, TArray<FLuaValue> Args)
{
	FLuaValue ReturnValue;
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return ReturnValue;

	L->FromLuaValue(Value);

	int NArgs = 0;
	for (FLuaValue& Arg : Args)
	{
		L->FromLuaValue(Arg);
		NArgs++;
	}

	L->PCall(NArgs, ReturnValue);

	L->Pop();

	return ReturnValue;
}

TArray<FLuaValue> ULuaBlueprintFunctionLibrary::LuaGlobalCallValueMulti(UObject* WorldContextObject, TSubclassOf<ULuaState> State, FLuaValue Value, TArray<FLuaValue> Args)
{
	TArray<FLuaValue> ReturnValue;
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return ReturnValue;

	L->FromLuaValue(Value);

	int32 StackTop = L->GetTop();

	int NArgs = 0;
	for (FLuaValue& Arg : Args)
	{
		L->FromLuaValue(Arg);
		NArgs++;
	}

	FLuaValue LastReturnValue;
	if (L->PCall(NArgs, LastReturnValue, LUA_MULTRET))
	{

		int32 NumOfReturnValues = (L->GetTop() - StackTop) + 1;
		if (NumOfReturnValues > 0)
		{
			for (int32 i = -1; i >= -(NumOfReturnValues); i--)
			{
				ReturnValue.Insert(L->ToLuaValue(i), 0);
			}
			L->Pop(NumOfReturnValues - 1);
		}

	}

	L->Pop();

	return ReturnValue;
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaValueCall(FLuaValue Value, TArray<FLuaValue> Args)
{
	FLuaValue ReturnValue;

	ULuaState* L = Value.LuaState;
	if (!L)
		return ReturnValue;

	L->FromLuaValue(Value);

	int NArgs = 0;
	for (FLuaValue& Arg : Args)
	{
		L->FromLuaValue(Arg);
		NArgs++;
	}

	L->PCall(NArgs, ReturnValue);

	L->Pop();

	return ReturnValue;
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaValueCallIfNotNil(FLuaValue Value, TArray<FLuaValue> Args)
{
	FLuaValue ReturnValue;
	if (Value.Type != ELuaValueType::Nil)
		ReturnValue = LuaValueCall(Value, Args);

	return ReturnValue;
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaTableKeyCall(FLuaValue InTable, FString Key, TArray<FLuaValue> Args)
{
	FLuaValue ReturnValue;
	if (InTable.Type != ELuaValueType::Table)
		return ReturnValue;

	ULuaState* L = InTable.LuaState;
	if (!L)
		return ReturnValue;

	FLuaValue Value = InTable.GetField(Key);
	if (Value.Type == ELuaValueType::Nil)
		return ReturnValue;

	return LuaValueCall(Value, Args);
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaTableIndexCall(FLuaValue InTable, int32 Index, TArray<FLuaValue> Args)
{
	FLuaValue ReturnValue;
	if (InTable.Type != ELuaValueType::Table)
		return ReturnValue;

	ULuaState* L = InTable.LuaState;
	if (!L)
		return ReturnValue;

	FLuaValue Value = InTable.GetFieldByIndex(Index);
	if (Value.Type == ELuaValueType::Nil)
		return ReturnValue;

	return LuaValueCall(Value, Args);
}

TArray<FLuaValue> ULuaBlueprintFunctionLibrary::LuaValueCallMulti(FLuaValue Value, TArray<FLuaValue> Args)
{
	TArray<FLuaValue> ReturnValue;

	ULuaState* L = Value.LuaState;
	if (!L)
		return ReturnValue;

	L->FromLuaValue(Value);

	int32 StackTop = L->GetTop();

	int NArgs = 0;
	for (FLuaValue& Arg : Args)
	{
		L->FromLuaValue(Arg);
		NArgs++;
	}

	FLuaValue LastReturnValue;
	if (L->PCall(NArgs, LastReturnValue, LUA_MULTRET))
	{

		int32 NumOfReturnValues = (L->GetTop() - StackTop) + 1;
		if (NumOfReturnValues > 0)
		{
			for (int32 i = -1; i >= -(NumOfReturnValues); i--)
			{
				ReturnValue.Insert(L->ToLuaValue(i), 0);
			}
			L->Pop(NumOfReturnValues - 1);
		}

	}

	L->Pop();

	return ReturnValue;
}

TArray<FLuaValue> ULuaBlueprintFunctionLibrary::LuaValueResumeMulti(FLuaValue Value, TArray<FLuaValue> Args)
{
	TArray<FLuaValue> ReturnValue;

	if (Value.Type != ELuaValueType::Thread)
		return ReturnValue;

	ULuaState* L = Value.LuaState;
	if (!L)
		return ReturnValue;

	L->FromLuaValue(Value);

	int32 StackTop = L->GetTop();

	int NArgs = 0;
	for (FLuaValue& Arg : Args)
	{
		L->FromLuaValue(Arg);
		NArgs++;
	}

	L->Resume(-1 - NArgs, NArgs);

	int32 NumOfReturnValues = (L->GetTop() - StackTop);
	if (NumOfReturnValues > 0)
	{
		for (int32 i = -1; i >= -(NumOfReturnValues); i--)
		{
			ReturnValue.Insert(L->ToLuaValue(i), 0);
		}
		L->Pop(NumOfReturnValues);
	}

	L->Pop();

	return ReturnValue;
}

int32 ULuaBlueprintFunctionLibrary::LuaValueLength(FLuaValue Value)
{

	ULuaState* L = Value.LuaState;
	if (!L)
		return 0;

	L->FromLuaValue(Value);
	L->Len(-1);
	int32 Length = L->ToInteger(-1);
	L->Pop(2);

	return Length;
}

TArray<FLuaValue> ULuaBlueprintFunctionLibrary::LuaTableGetKeys(FLuaValue Table)
{
	TArray<FLuaValue> Keys;

	if (Table.Type != ELuaValueType::Table)
		return Keys;

	ULuaState* L = Table.LuaState;
	if (!L)
		return Keys;

	L->FromLuaValue(Table);
	L->PushNil(); // first key
	while (L->Next(-2))
	{
		Keys.Add(L->ToLuaValue(-2)); // add key
		L->Pop(); // pop the value
	}

	L->Pop(); // pop the table

	return Keys;
}

TArray<FLuaValue> ULuaBlueprintFunctionLibrary::LuaTableGetValues(FLuaValue Table)
{
	TArray<FLuaValue> Keys;

	if (Table.Type != ELuaValueType::Table)
		return Keys;

	ULuaState* L = Table.LuaState;
	if (!L)
		return Keys;

	L->FromLuaValue(Table);
	L->PushNil(); // first key
	while (L->Next(-2))
	{
		Keys.Add(L->ToLuaValue(-1)); // add value
		L->Pop(); // pop the value
	}

	L->Pop(); // pop the table

	return Keys;
}

int32 ULuaBlueprintFunctionLibrary::LuaGetUsedMemory(UObject* WorldContextObject, TSubclassOf<ULuaState> State)
{
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return -1;

	return L->GC(LUA_GCCOUNT);
}

void ULuaBlueprintFunctionLibrary::LuaGCCollect(UObject* WorldContextObject, TSubclassOf<ULuaState> State)
{
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return;

	L->GC(LUA_GCCOLLECT);
}

void ULuaBlueprintFunctionLibrary::LuaGCStop(UObject* WorldContextObject, TSubclassOf<ULuaState> State)
{
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return;

	L->GC(LUA_GCSTOP);
}

void ULuaBlueprintFunctionLibrary::LuaGCRestart(UObject* WorldContextObject, TSubclassOf<ULuaState> State)
{
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return;

	L->GC(LUA_GCRESTART);
}

void ULuaBlueprintFunctionLibrary::LuaSetUserDataMetaTable(UObject* WorldContextObject, TSubclassOf<ULuaState> State, FLuaValue MetaTable)
{
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return;
	L->SetUserDataMetaTable(MetaTable);
}