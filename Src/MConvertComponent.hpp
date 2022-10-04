//---------------------------------------------------------------------------

#ifndef MConvertComponentH
#define MConvertComponentH
//---------------------------------------------------------------------------

#include <memory>

namespace MUtils {

	// ---------------------------------------------------------------------------
	const UnicodeString ComponentToString(TComponent *AComponent) {

		std::auto_ptr<TMemoryStream>AInStream(new TMemoryStream());
		AInStream->WriteComponent(AComponent);
		AInStream->Position = 0;

		std::auto_ptr<TStringStream>AOutStream(new TStringStream());
		ObjectBinaryToText(AInStream.get(), AOutStream.get());
		AOutStream->Position = 0;

		return AOutStream->DataString;
	}

	// ---------------------------------------------------------------------------
	TComponent * StringToComponent(TComponent *AComponent, const UnicodeString &sText) {

		std::auto_ptr<TStringStream>AInStream(new TStringStream(sText));
		AInStream->Position = 0;
		std::auto_ptr<TMemoryStream>AOutStream(new TMemoryStream());

		ObjectTextToBinary(AInStream.get(), AOutStream.get());
		AOutStream->Position = 0;

		return AOutStream->ReadComponent(AComponent);

	}

	// ---------------------------------------------------------------------------
	void SaveComponentToTextFile(TComponent *AComponent, const UnicodeString &sFileName) {

		std::auto_ptr<TMemoryStream>AInStream(new TMemoryStream());
		AInStream->WriteComponent(AComponent);
		AInStream->Position = 0;

		std::auto_ptr<TMemoryStream>AOutStream(new TMemoryStream());
		ObjectBinaryToText(AInStream.get(), AOutStream.get());
		AOutStream->Position = 0;

		AOutStream->SaveToFile(sFileName);
	}

	// ---------------------------------------------------------------------------
	TComponent * LoadComponentFromTextFile(TComponent *AComponent, const UnicodeString &sFileName) {

		std::auto_ptr<TMemoryStream>AInStream(new TMemoryStream());
		AInStream->LoadFromFile(sFileName);
		AInStream->Position = 0;
		std::auto_ptr<TMemoryStream>AOutStream(new TMemoryStream());

		ObjectTextToBinary(AInStream.get(), AOutStream.get());
		AOutStream->Position = 0;

		return AOutStream->ReadComponent(AComponent);

	}

	// ---------------------------------------------------------------------------
	TComponent* LoadComponent(const UnicodeString &sFileName, TComponent* AComponent, const Classes::TReaderError OnReaderError)
	{
		std::auto_ptr<TMemoryStream>AInStream(new TMemoryStream());
		AInStream->LoadFromFile(sFileName);
		AInStream->Position = 0;
		std::auto_ptr<TMemoryStream>AOutStream(new TMemoryStream());

		ObjectTextToBinary(AInStream.get(), AOutStream.get());
		AOutStream->Position = 0;

		std::auto_ptr<TReader>Reader(new TReader(AOutStream.get(), 4096));
		Reader->OnError = OnReaderError;

		return Reader->ReadRootComponent(AComponent);
	}

};

#endif

