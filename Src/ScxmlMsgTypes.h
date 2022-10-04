#ifndef __SCXML_MSG_TYPES__
#define __SCXML_MSG_TYPES__

typedef enum {
	smttUnknown, smttAfterEnter, smttBeforeEnter, smttAfterExit, smttBeforeExit, smttStep, smttBeforeExecContent, smttAfterExecContent,
	smttBeforeInvoke, smttAfterInvoke, smttBeforeUnInvoke, smttAfterUnInvoke, smttBeforeTakingTransition, smttAfterTakingTransition,
	smttStableConfiguration, smttBeforeProcessingEvent, smttMAXSIZE
}TScxmlMsgType;
typedef Set<TScxmlMsgType, smttUnknown, smttMAXSIZE> TScxmlMsgTypes;

#define DEFAULT_SETTINGS_SCXML_MSG_TYPES TScxmlMsgTypes() << smttBeforeEnter << smttBeforeExit << smttBeforeExecContent << smttBeforeInvoke << smttBeforeUnInvoke << smttBeforeTakingTransition << smttBeforeProcessingEvent

UnicodeString ScxmlMsgTypeToString(const TScxmlMsgType AType) {
	switch (AType) {
	case smttAfterEnter: 					return L"smttAfterEnter";
	case smttBeforeEnter: 					return L"smttBeforeEnter";
	case smttAfterExit: 					return L"smttAfterExit";
	case smttBeforeExit: 					return L"smttBeforeExit";
	case smttBeforeExecContent: 			return L"smttBeforeExecContent";
	case smttAfterExecContent: 				return L"smttAfterExecContent";
	case smttBeforeInvoke: 					return L"smttBeforeInvoke";
	case smttAfterInvoke: 					return L"smttAfterInvoke";
	case smttBeforeUnInvoke: 				return L"smttBeforeUnInvoke";
	case smttAfterUnInvoke: 				return L"smttAfterUnInvoke";
	case smttBeforeTakingTransition: 		return L"smttBeforeTakingTransition";
	case smttAfterTakingTransition: 		return L"smttAfterTakingTransition";
	case smttStep: 							return L"smttStep";
	case smttStableConfiguration: 			return L"smttStableConfiguration";
	case smttBeforeProcessingEvent: 		return L"smttBeforeProcessingEvent";
	}

	return L"smttUnknown";
}

TScxmlMsgType StringToScxmlMsgType(const UnicodeString &sText) {
	if (sText==L"smttAfterEnter") 					return smttAfterEnter;
	if (sText==L"smttBeforeEnter") 					return smttBeforeEnter;
	if (sText==L"smttAfterExit") 					return smttAfterExit;
	if (sText==L"smttBeforeExit") 					return smttBeforeExit;
	if (sText==L"smttBeforeExecContent") 			return smttBeforeExecContent;
	if (sText==L"smttAfterExecContent") 			return smttAfterExecContent;
	if (sText==L"smttBeforeInvoke") 				return smttBeforeInvoke;
	if (sText==L"smttAfterInvoke") 					return smttAfterInvoke;
	if (sText==L"smttBeforeUnInvoke") 				return smttBeforeUnInvoke;
	if (sText==L"smttAfterUnInvoke") 				return smttAfterUnInvoke;
	if (sText==L"smttBeforeTakingTransition") 		return smttBeforeTakingTransition;
	if (sText==L"smttAfterTakingTransition") 		return smttAfterTakingTransition;
	if (sText==L"smttStep") 						return smttStep;
	if (sText==L"smttStableConfiguration") 			return smttStableConfiguration;
	if (sText==L"smttBeforeProcessingEvent") 		return smttBeforeProcessingEvent;

	return smttUnknown;
}

#endif /* __SCXML_MSG_TYPES__ */
