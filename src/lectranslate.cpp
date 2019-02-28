#include "extension.h"
#include "LEC.h"

std::mutex procMutex;

bool ProcessSentence(std::wstring& sentence, SentenceInfo sentenceInfo)
{
	if (!sentenceInfo["text number"] || !sentenceInfo["current select"])  {
		return false;
	}

	std::lock_guard<std::mutex> lock(procMutex);
	if (!lecState) {
		SetUpLEC();
		if (lecState < 0) {
			return false;
		}
	}
	if (lecState < 0) {
		return false;
	}
	wchar_t *otext = wcsdup(sentence.c_str());
	wchar_t *text = LECTranslateFull(otext);
	sentence += L"\n";
	sentence += text;
	free(otext);
	free(text);
	return true;
}

