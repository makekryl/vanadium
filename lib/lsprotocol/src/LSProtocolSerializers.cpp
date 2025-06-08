#include "LSProtocol.h"
#include <cassert>
#include <ryml.hpp>

namespace lsp {

template <>
void detail::SerializeEnum(SemanticTokenTypes val, ryml::NodeRef n) {
  switch (val) {
    case SemanticTokenTypes::kNamespace: n = "namespace"; break;
    case SemanticTokenTypes::kType: n = "type"; break;
    case SemanticTokenTypes::kClass: n = "class"; break;
    case SemanticTokenTypes::kEnum: n = "enum"; break;
    case SemanticTokenTypes::kInterface: n = "interface"; break;
    case SemanticTokenTypes::kStruct: n = "struct"; break;
    case SemanticTokenTypes::kTypeparameter: n = "typeParameter"; break;
    case SemanticTokenTypes::kParameter: n = "parameter"; break;
    case SemanticTokenTypes::kVariable: n = "variable"; break;
    case SemanticTokenTypes::kProperty: n = "property"; break;
    case SemanticTokenTypes::kEnummember: n = "enumMember"; break;
    case SemanticTokenTypes::kEvent: n = "event"; break;
    case SemanticTokenTypes::kFunction: n = "function"; break;
    case SemanticTokenTypes::kMethod: n = "method"; break;
    case SemanticTokenTypes::kMacro: n = "macro"; break;
    case SemanticTokenTypes::kKeyword: n = "keyword"; break;
    case SemanticTokenTypes::kModifier: n = "modifier"; break;
    case SemanticTokenTypes::kComment: n = "comment"; break;
    case SemanticTokenTypes::kString: n = "string"; break;
    case SemanticTokenTypes::kNumber: n = "number"; break;
    case SemanticTokenTypes::kRegexp: n = "regexp"; break;
    case SemanticTokenTypes::kOperator: n = "operator"; break;
    case SemanticTokenTypes::kDecorator: n = "decorator"; break;
    case SemanticTokenTypes::kLabel: n = "label"; break;
  }
}

template <>
SemanticTokenTypes detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("namespace" == tv) { return SemanticTokenTypes::kNamespace; }
  if ("type" == tv) { return SemanticTokenTypes::kType; }
  if ("class" == tv) { return SemanticTokenTypes::kClass; }
  if ("enum" == tv) { return SemanticTokenTypes::kEnum; }
  if ("interface" == tv) { return SemanticTokenTypes::kInterface; }
  if ("struct" == tv) { return SemanticTokenTypes::kStruct; }
  if ("typeParameter" == tv) { return SemanticTokenTypes::kTypeparameter; }
  if ("parameter" == tv) { return SemanticTokenTypes::kParameter; }
  if ("variable" == tv) { return SemanticTokenTypes::kVariable; }
  if ("property" == tv) { return SemanticTokenTypes::kProperty; }
  if ("enumMember" == tv) { return SemanticTokenTypes::kEnummember; }
  if ("event" == tv) { return SemanticTokenTypes::kEvent; }
  if ("function" == tv) { return SemanticTokenTypes::kFunction; }
  if ("method" == tv) { return SemanticTokenTypes::kMethod; }
  if ("macro" == tv) { return SemanticTokenTypes::kMacro; }
  if ("keyword" == tv) { return SemanticTokenTypes::kKeyword; }
  if ("modifier" == tv) { return SemanticTokenTypes::kModifier; }
  if ("comment" == tv) { return SemanticTokenTypes::kComment; }
  if ("string" == tv) { return SemanticTokenTypes::kString; }
  if ("number" == tv) { return SemanticTokenTypes::kNumber; }
  if ("regexp" == tv) { return SemanticTokenTypes::kRegexp; }
  if ("operator" == tv) { return SemanticTokenTypes::kOperator; }
  if ("decorator" == tv) { return SemanticTokenTypes::kDecorator; }
  if ("label" == tv) { return SemanticTokenTypes::kLabel; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(SemanticTokenModifiers val, ryml::NodeRef n) {
  switch (val) {
    case SemanticTokenModifiers::kDeclaration: n = "declaration"; break;
    case SemanticTokenModifiers::kDefinition: n = "definition"; break;
    case SemanticTokenModifiers::kReadonly: n = "readonly"; break;
    case SemanticTokenModifiers::kStatic: n = "static"; break;
    case SemanticTokenModifiers::kDeprecated: n = "deprecated"; break;
    case SemanticTokenModifiers::kAbstract: n = "abstract"; break;
    case SemanticTokenModifiers::kAsync: n = "async"; break;
    case SemanticTokenModifiers::kModification: n = "modification"; break;
    case SemanticTokenModifiers::kDocumentation: n = "documentation"; break;
    case SemanticTokenModifiers::kDefaultlibrary: n = "defaultLibrary"; break;
  }
}

template <>
SemanticTokenModifiers detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("declaration" == tv) { return SemanticTokenModifiers::kDeclaration; }
  if ("definition" == tv) { return SemanticTokenModifiers::kDefinition; }
  if ("readonly" == tv) { return SemanticTokenModifiers::kReadonly; }
  if ("static" == tv) { return SemanticTokenModifiers::kStatic; }
  if ("deprecated" == tv) { return SemanticTokenModifiers::kDeprecated; }
  if ("abstract" == tv) { return SemanticTokenModifiers::kAbstract; }
  if ("async" == tv) { return SemanticTokenModifiers::kAsync; }
  if ("modification" == tv) { return SemanticTokenModifiers::kModification; }
  if ("documentation" == tv) { return SemanticTokenModifiers::kDocumentation; }
  if ("defaultLibrary" == tv) { return SemanticTokenModifiers::kDefaultlibrary; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(DocumentDiagnosticReportKind val, ryml::NodeRef n) {
  switch (val) {
    case DocumentDiagnosticReportKind::kFull: n = "full"; break;
    case DocumentDiagnosticReportKind::kUnchanged: n = "unchanged"; break;
  }
}

template <>
DocumentDiagnosticReportKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("full" == tv) { return DocumentDiagnosticReportKind::kFull; }
  if ("unchanged" == tv) { return DocumentDiagnosticReportKind::kUnchanged; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(ErrorCodes val, ryml::NodeRef n) {
  switch (val) {
    case ErrorCodes::kParseerror: n = "-32700"; break;
    case ErrorCodes::kInvalidrequest: n = "-32600"; break;
    case ErrorCodes::kMethodnotfound: n = "-32601"; break;
    case ErrorCodes::kInvalidparams: n = "-32602"; break;
    case ErrorCodes::kInternalerror: n = "-32603"; break;
    case ErrorCodes::kServernotinitialized: n = "-32002"; break;
    case ErrorCodes::kUnknownerrorcode: n = "-32001"; break;
  }
}

template <>
ErrorCodes detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("-32700" == tv) { return ErrorCodes::kParseerror; }
  if ("-32600" == tv) { return ErrorCodes::kInvalidrequest; }
  if ("-32601" == tv) { return ErrorCodes::kMethodnotfound; }
  if ("-32602" == tv) { return ErrorCodes::kInvalidparams; }
  if ("-32603" == tv) { return ErrorCodes::kInternalerror; }
  if ("-32002" == tv) { return ErrorCodes::kServernotinitialized; }
  if ("-32001" == tv) { return ErrorCodes::kUnknownerrorcode; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(LSPErrorCodes val, ryml::NodeRef n) {
  switch (val) {
    case LSPErrorCodes::kRequestfailed: n = "-32803"; break;
    case LSPErrorCodes::kServercancelled: n = "-32802"; break;
    case LSPErrorCodes::kContentmodified: n = "-32801"; break;
    case LSPErrorCodes::kRequestcancelled: n = "-32800"; break;
  }
}

template <>
LSPErrorCodes detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("-32803" == tv) { return LSPErrorCodes::kRequestfailed; }
  if ("-32802" == tv) { return LSPErrorCodes::kServercancelled; }
  if ("-32801" == tv) { return LSPErrorCodes::kContentmodified; }
  if ("-32800" == tv) { return LSPErrorCodes::kRequestcancelled; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(FoldingRangeKind val, ryml::NodeRef n) {
  switch (val) {
    case FoldingRangeKind::kComment: n = "comment"; break;
    case FoldingRangeKind::kImports: n = "imports"; break;
    case FoldingRangeKind::kRegion: n = "region"; break;
  }
}

template <>
FoldingRangeKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("comment" == tv) { return FoldingRangeKind::kComment; }
  if ("imports" == tv) { return FoldingRangeKind::kImports; }
  if ("region" == tv) { return FoldingRangeKind::kRegion; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(SymbolKind val, ryml::NodeRef n) {
  switch (val) {
    case SymbolKind::kFile: n = "1"; break;
    case SymbolKind::kModule: n = "2"; break;
    case SymbolKind::kNamespace: n = "3"; break;
    case SymbolKind::kPackage: n = "4"; break;
    case SymbolKind::kClass: n = "5"; break;
    case SymbolKind::kMethod: n = "6"; break;
    case SymbolKind::kProperty: n = "7"; break;
    case SymbolKind::kField: n = "8"; break;
    case SymbolKind::kConstructor: n = "9"; break;
    case SymbolKind::kEnum: n = "10"; break;
    case SymbolKind::kInterface: n = "11"; break;
    case SymbolKind::kFunction: n = "12"; break;
    case SymbolKind::kVariable: n = "13"; break;
    case SymbolKind::kConstant: n = "14"; break;
    case SymbolKind::kString: n = "15"; break;
    case SymbolKind::kNumber: n = "16"; break;
    case SymbolKind::kBoolean: n = "17"; break;
    case SymbolKind::kArray: n = "18"; break;
    case SymbolKind::kObject: n = "19"; break;
    case SymbolKind::kKey: n = "20"; break;
    case SymbolKind::kNull: n = "21"; break;
    case SymbolKind::kEnummember: n = "22"; break;
    case SymbolKind::kStruct: n = "23"; break;
    case SymbolKind::kEvent: n = "24"; break;
    case SymbolKind::kOperator: n = "25"; break;
    case SymbolKind::kTypeparameter: n = "26"; break;
  }
}

template <>
SymbolKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return SymbolKind::kFile; }
  if ("2" == tv) { return SymbolKind::kModule; }
  if ("3" == tv) { return SymbolKind::kNamespace; }
  if ("4" == tv) { return SymbolKind::kPackage; }
  if ("5" == tv) { return SymbolKind::kClass; }
  if ("6" == tv) { return SymbolKind::kMethod; }
  if ("7" == tv) { return SymbolKind::kProperty; }
  if ("8" == tv) { return SymbolKind::kField; }
  if ("9" == tv) { return SymbolKind::kConstructor; }
  if ("10" == tv) { return SymbolKind::kEnum; }
  if ("11" == tv) { return SymbolKind::kInterface; }
  if ("12" == tv) { return SymbolKind::kFunction; }
  if ("13" == tv) { return SymbolKind::kVariable; }
  if ("14" == tv) { return SymbolKind::kConstant; }
  if ("15" == tv) { return SymbolKind::kString; }
  if ("16" == tv) { return SymbolKind::kNumber; }
  if ("17" == tv) { return SymbolKind::kBoolean; }
  if ("18" == tv) { return SymbolKind::kArray; }
  if ("19" == tv) { return SymbolKind::kObject; }
  if ("20" == tv) { return SymbolKind::kKey; }
  if ("21" == tv) { return SymbolKind::kNull; }
  if ("22" == tv) { return SymbolKind::kEnummember; }
  if ("23" == tv) { return SymbolKind::kStruct; }
  if ("24" == tv) { return SymbolKind::kEvent; }
  if ("25" == tv) { return SymbolKind::kOperator; }
  if ("26" == tv) { return SymbolKind::kTypeparameter; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(SymbolTag val, ryml::NodeRef n) {
  switch (val) {
    case SymbolTag::kDeprecated: n = "1"; break;
  }
}

template <>
SymbolTag detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return SymbolTag::kDeprecated; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(UniquenessLevel val, ryml::NodeRef n) {
  switch (val) {
    case UniquenessLevel::kDocument: n = "document"; break;
    case UniquenessLevel::kProject: n = "project"; break;
    case UniquenessLevel::kGroup: n = "group"; break;
    case UniquenessLevel::kScheme: n = "scheme"; break;
    case UniquenessLevel::kGlobal: n = "global"; break;
  }
}

template <>
UniquenessLevel detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("document" == tv) { return UniquenessLevel::kDocument; }
  if ("project" == tv) { return UniquenessLevel::kProject; }
  if ("group" == tv) { return UniquenessLevel::kGroup; }
  if ("scheme" == tv) { return UniquenessLevel::kScheme; }
  if ("global" == tv) { return UniquenessLevel::kGlobal; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(MonikerKind val, ryml::NodeRef n) {
  switch (val) {
    case MonikerKind::kImport: n = "import"; break;
    case MonikerKind::kExport: n = "export"; break;
    case MonikerKind::kLocal: n = "local"; break;
  }
}

template <>
MonikerKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("import" == tv) { return MonikerKind::kImport; }
  if ("export" == tv) { return MonikerKind::kExport; }
  if ("local" == tv) { return MonikerKind::kLocal; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(InlayHintKind val, ryml::NodeRef n) {
  switch (val) {
    case InlayHintKind::kType: n = "1"; break;
    case InlayHintKind::kParameter: n = "2"; break;
  }
}

template <>
InlayHintKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return InlayHintKind::kType; }
  if ("2" == tv) { return InlayHintKind::kParameter; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(MessageType val, ryml::NodeRef n) {
  switch (val) {
    case MessageType::kError: n = "1"; break;
    case MessageType::kWarning: n = "2"; break;
    case MessageType::kInfo: n = "3"; break;
    case MessageType::kLog: n = "4"; break;
    case MessageType::kDebug: n = "5"; break;
  }
}

template <>
MessageType detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return MessageType::kError; }
  if ("2" == tv) { return MessageType::kWarning; }
  if ("3" == tv) { return MessageType::kInfo; }
  if ("4" == tv) { return MessageType::kLog; }
  if ("5" == tv) { return MessageType::kDebug; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(TextDocumentSyncKind val, ryml::NodeRef n) {
  switch (val) {
    case TextDocumentSyncKind::kNone: n = "0"; break;
    case TextDocumentSyncKind::kFull: n = "1"; break;
    case TextDocumentSyncKind::kIncremental: n = "2"; break;
  }
}

template <>
TextDocumentSyncKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("0" == tv) { return TextDocumentSyncKind::kNone; }
  if ("1" == tv) { return TextDocumentSyncKind::kFull; }
  if ("2" == tv) { return TextDocumentSyncKind::kIncremental; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(TextDocumentSaveReason val, ryml::NodeRef n) {
  switch (val) {
    case TextDocumentSaveReason::kManual: n = "1"; break;
    case TextDocumentSaveReason::kAfterdelay: n = "2"; break;
    case TextDocumentSaveReason::kFocusout: n = "3"; break;
  }
}

template <>
TextDocumentSaveReason detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return TextDocumentSaveReason::kManual; }
  if ("2" == tv) { return TextDocumentSaveReason::kAfterdelay; }
  if ("3" == tv) { return TextDocumentSaveReason::kFocusout; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(CompletionItemKind val, ryml::NodeRef n) {
  switch (val) {
    case CompletionItemKind::kText: n = "1"; break;
    case CompletionItemKind::kMethod: n = "2"; break;
    case CompletionItemKind::kFunction: n = "3"; break;
    case CompletionItemKind::kConstructor: n = "4"; break;
    case CompletionItemKind::kField: n = "5"; break;
    case CompletionItemKind::kVariable: n = "6"; break;
    case CompletionItemKind::kClass: n = "7"; break;
    case CompletionItemKind::kInterface: n = "8"; break;
    case CompletionItemKind::kModule: n = "9"; break;
    case CompletionItemKind::kProperty: n = "10"; break;
    case CompletionItemKind::kUnit: n = "11"; break;
    case CompletionItemKind::kValue: n = "12"; break;
    case CompletionItemKind::kEnum: n = "13"; break;
    case CompletionItemKind::kKeyword: n = "14"; break;
    case CompletionItemKind::kSnippet: n = "15"; break;
    case CompletionItemKind::kColor: n = "16"; break;
    case CompletionItemKind::kFile: n = "17"; break;
    case CompletionItemKind::kReference: n = "18"; break;
    case CompletionItemKind::kFolder: n = "19"; break;
    case CompletionItemKind::kEnummember: n = "20"; break;
    case CompletionItemKind::kConstant: n = "21"; break;
    case CompletionItemKind::kStruct: n = "22"; break;
    case CompletionItemKind::kEvent: n = "23"; break;
    case CompletionItemKind::kOperator: n = "24"; break;
    case CompletionItemKind::kTypeparameter: n = "25"; break;
  }
}

template <>
CompletionItemKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return CompletionItemKind::kText; }
  if ("2" == tv) { return CompletionItemKind::kMethod; }
  if ("3" == tv) { return CompletionItemKind::kFunction; }
  if ("4" == tv) { return CompletionItemKind::kConstructor; }
  if ("5" == tv) { return CompletionItemKind::kField; }
  if ("6" == tv) { return CompletionItemKind::kVariable; }
  if ("7" == tv) { return CompletionItemKind::kClass; }
  if ("8" == tv) { return CompletionItemKind::kInterface; }
  if ("9" == tv) { return CompletionItemKind::kModule; }
  if ("10" == tv) { return CompletionItemKind::kProperty; }
  if ("11" == tv) { return CompletionItemKind::kUnit; }
  if ("12" == tv) { return CompletionItemKind::kValue; }
  if ("13" == tv) { return CompletionItemKind::kEnum; }
  if ("14" == tv) { return CompletionItemKind::kKeyword; }
  if ("15" == tv) { return CompletionItemKind::kSnippet; }
  if ("16" == tv) { return CompletionItemKind::kColor; }
  if ("17" == tv) { return CompletionItemKind::kFile; }
  if ("18" == tv) { return CompletionItemKind::kReference; }
  if ("19" == tv) { return CompletionItemKind::kFolder; }
  if ("20" == tv) { return CompletionItemKind::kEnummember; }
  if ("21" == tv) { return CompletionItemKind::kConstant; }
  if ("22" == tv) { return CompletionItemKind::kStruct; }
  if ("23" == tv) { return CompletionItemKind::kEvent; }
  if ("24" == tv) { return CompletionItemKind::kOperator; }
  if ("25" == tv) { return CompletionItemKind::kTypeparameter; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(CompletionItemTag val, ryml::NodeRef n) {
  switch (val) {
    case CompletionItemTag::kDeprecated: n = "1"; break;
  }
}

template <>
CompletionItemTag detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return CompletionItemTag::kDeprecated; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(InsertTextFormat val, ryml::NodeRef n) {
  switch (val) {
    case InsertTextFormat::kPlaintext: n = "1"; break;
    case InsertTextFormat::kSnippet: n = "2"; break;
  }
}

template <>
InsertTextFormat detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return InsertTextFormat::kPlaintext; }
  if ("2" == tv) { return InsertTextFormat::kSnippet; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(InsertTextMode val, ryml::NodeRef n) {
  switch (val) {
    case InsertTextMode::kAsis: n = "1"; break;
    case InsertTextMode::kAdjustindentation: n = "2"; break;
  }
}

template <>
InsertTextMode detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return InsertTextMode::kAsis; }
  if ("2" == tv) { return InsertTextMode::kAdjustindentation; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(DocumentHighlightKind val, ryml::NodeRef n) {
  switch (val) {
    case DocumentHighlightKind::kText: n = "1"; break;
    case DocumentHighlightKind::kRead: n = "2"; break;
    case DocumentHighlightKind::kWrite: n = "3"; break;
  }
}

template <>
DocumentHighlightKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return DocumentHighlightKind::kText; }
  if ("2" == tv) { return DocumentHighlightKind::kRead; }
  if ("3" == tv) { return DocumentHighlightKind::kWrite; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(CodeActionKind val, ryml::NodeRef n) {
  switch (val) {
    case CodeActionKind::kEmpty: n = ""; break;
    case CodeActionKind::kQuickfix: n = "quickfix"; break;
    case CodeActionKind::kRefactor: n = "refactor"; break;
    case CodeActionKind::kRefactorextract: n = "refactor.extract"; break;
    case CodeActionKind::kRefactorinline: n = "refactor.inline"; break;
    case CodeActionKind::kRefactormove: n = "refactor.move"; break;
    case CodeActionKind::kRefactorrewrite: n = "refactor.rewrite"; break;
    case CodeActionKind::kSource: n = "source"; break;
    case CodeActionKind::kSourceorganizeimports: n = "source.organizeImports"; break;
    case CodeActionKind::kSourcefixall: n = "source.fixAll"; break;
    case CodeActionKind::kNotebook: n = "notebook"; break;
  }
}

template <>
CodeActionKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("" == tv) { return CodeActionKind::kEmpty; }
  if ("quickfix" == tv) { return CodeActionKind::kQuickfix; }
  if ("refactor" == tv) { return CodeActionKind::kRefactor; }
  if ("refactor.extract" == tv) { return CodeActionKind::kRefactorextract; }
  if ("refactor.inline" == tv) { return CodeActionKind::kRefactorinline; }
  if ("refactor.move" == tv) { return CodeActionKind::kRefactormove; }
  if ("refactor.rewrite" == tv) { return CodeActionKind::kRefactorrewrite; }
  if ("source" == tv) { return CodeActionKind::kSource; }
  if ("source.organizeImports" == tv) { return CodeActionKind::kSourceorganizeimports; }
  if ("source.fixAll" == tv) { return CodeActionKind::kSourcefixall; }
  if ("notebook" == tv) { return CodeActionKind::kNotebook; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(CodeActionTag val, ryml::NodeRef n) {
  switch (val) {
    case CodeActionTag::kLlmgenerated: n = "1"; break;
  }
}

template <>
CodeActionTag detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return CodeActionTag::kLlmgenerated; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(TraceValue val, ryml::NodeRef n) {
  switch (val) {
    case TraceValue::kOff: n = "off"; break;
    case TraceValue::kMessages: n = "messages"; break;
    case TraceValue::kVerbose: n = "verbose"; break;
  }
}

template <>
TraceValue detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("off" == tv) { return TraceValue::kOff; }
  if ("messages" == tv) { return TraceValue::kMessages; }
  if ("verbose" == tv) { return TraceValue::kVerbose; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(MarkupKind val, ryml::NodeRef n) {
  switch (val) {
    case MarkupKind::kPlaintext: n = "plaintext"; break;
    case MarkupKind::kMarkdown: n = "markdown"; break;
  }
}

template <>
MarkupKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("plaintext" == tv) { return MarkupKind::kPlaintext; }
  if ("markdown" == tv) { return MarkupKind::kMarkdown; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(LanguageKind val, ryml::NodeRef n) {
  switch (val) {
    case LanguageKind::kAbap: n = "abap"; break;
    case LanguageKind::kWindowsbat: n = "bat"; break;
    case LanguageKind::kBibtex: n = "bibtex"; break;
    case LanguageKind::kClojure: n = "clojure"; break;
    case LanguageKind::kCoffeescript: n = "coffeescript"; break;
    case LanguageKind::kC: n = "c"; break;
    case LanguageKind::kCpp: n = "cpp"; break;
    case LanguageKind::kCsharp: n = "csharp"; break;
    case LanguageKind::kCss: n = "css"; break;
    case LanguageKind::kD: n = "d"; break;
    case LanguageKind::kDelphi: n = "pascal"; break;
    case LanguageKind::kDiff: n = "diff"; break;
    case LanguageKind::kDart: n = "dart"; break;
    case LanguageKind::kDockerfile: n = "dockerfile"; break;
    case LanguageKind::kElixir: n = "elixir"; break;
    case LanguageKind::kErlang: n = "erlang"; break;
    case LanguageKind::kFsharp: n = "fsharp"; break;
    case LanguageKind::kGitcommit: n = "git-commit"; break;
    case LanguageKind::kGitrebase: n = "rebase"; break;
    case LanguageKind::kGo: n = "go"; break;
    case LanguageKind::kGroovy: n = "groovy"; break;
    case LanguageKind::kHandlebars: n = "handlebars"; break;
    case LanguageKind::kHaskell: n = "haskell"; break;
    case LanguageKind::kHtml: n = "html"; break;
    case LanguageKind::kIni: n = "ini"; break;
    case LanguageKind::kJava: n = "java"; break;
    case LanguageKind::kJavascript: n = "javascript"; break;
    case LanguageKind::kJavascriptreact: n = "javascriptreact"; break;
    case LanguageKind::kJson: n = "json"; break;
    case LanguageKind::kLatex: n = "latex"; break;
    case LanguageKind::kLess: n = "less"; break;
    case LanguageKind::kLua: n = "lua"; break;
    case LanguageKind::kMakefile: n = "makefile"; break;
    case LanguageKind::kMarkdown: n = "markdown"; break;
    case LanguageKind::kObjectivec: n = "objective-c"; break;
    case LanguageKind::kObjectivecpp: n = "objective-cpp"; break;
    case LanguageKind::kPascal: n = "pascal"; break;
    case LanguageKind::kPerl: n = "perl"; break;
    case LanguageKind::kPerl6: n = "perl6"; break;
    case LanguageKind::kPhp: n = "php"; break;
    case LanguageKind::kPowershell: n = "powershell"; break;
    case LanguageKind::kPug: n = "jade"; break;
    case LanguageKind::kPython: n = "python"; break;
    case LanguageKind::kR: n = "r"; break;
    case LanguageKind::kRazor: n = "razor"; break;
    case LanguageKind::kRuby: n = "ruby"; break;
    case LanguageKind::kRust: n = "rust"; break;
    case LanguageKind::kScss: n = "scss"; break;
    case LanguageKind::kSass: n = "sass"; break;
    case LanguageKind::kScala: n = "scala"; break;
    case LanguageKind::kShaderlab: n = "shaderlab"; break;
    case LanguageKind::kShellscript: n = "shellscript"; break;
    case LanguageKind::kSql: n = "sql"; break;
    case LanguageKind::kSwift: n = "swift"; break;
    case LanguageKind::kTypescript: n = "typescript"; break;
    case LanguageKind::kTypescriptreact: n = "typescriptreact"; break;
    case LanguageKind::kTex: n = "tex"; break;
    case LanguageKind::kVisualbasic: n = "vb"; break;
    case LanguageKind::kXml: n = "xml"; break;
    case LanguageKind::kXsl: n = "xsl"; break;
    case LanguageKind::kYaml: n = "yaml"; break;
  }
}

template <>
LanguageKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("abap" == tv) { return LanguageKind::kAbap; }
  if ("bat" == tv) { return LanguageKind::kWindowsbat; }
  if ("bibtex" == tv) { return LanguageKind::kBibtex; }
  if ("clojure" == tv) { return LanguageKind::kClojure; }
  if ("coffeescript" == tv) { return LanguageKind::kCoffeescript; }
  if ("c" == tv) { return LanguageKind::kC; }
  if ("cpp" == tv) { return LanguageKind::kCpp; }
  if ("csharp" == tv) { return LanguageKind::kCsharp; }
  if ("css" == tv) { return LanguageKind::kCss; }
  if ("d" == tv) { return LanguageKind::kD; }
  if ("pascal" == tv) { return LanguageKind::kDelphi; }
  if ("diff" == tv) { return LanguageKind::kDiff; }
  if ("dart" == tv) { return LanguageKind::kDart; }
  if ("dockerfile" == tv) { return LanguageKind::kDockerfile; }
  if ("elixir" == tv) { return LanguageKind::kElixir; }
  if ("erlang" == tv) { return LanguageKind::kErlang; }
  if ("fsharp" == tv) { return LanguageKind::kFsharp; }
  if ("git-commit" == tv) { return LanguageKind::kGitcommit; }
  if ("rebase" == tv) { return LanguageKind::kGitrebase; }
  if ("go" == tv) { return LanguageKind::kGo; }
  if ("groovy" == tv) { return LanguageKind::kGroovy; }
  if ("handlebars" == tv) { return LanguageKind::kHandlebars; }
  if ("haskell" == tv) { return LanguageKind::kHaskell; }
  if ("html" == tv) { return LanguageKind::kHtml; }
  if ("ini" == tv) { return LanguageKind::kIni; }
  if ("java" == tv) { return LanguageKind::kJava; }
  if ("javascript" == tv) { return LanguageKind::kJavascript; }
  if ("javascriptreact" == tv) { return LanguageKind::kJavascriptreact; }
  if ("json" == tv) { return LanguageKind::kJson; }
  if ("latex" == tv) { return LanguageKind::kLatex; }
  if ("less" == tv) { return LanguageKind::kLess; }
  if ("lua" == tv) { return LanguageKind::kLua; }
  if ("makefile" == tv) { return LanguageKind::kMakefile; }
  if ("markdown" == tv) { return LanguageKind::kMarkdown; }
  if ("objective-c" == tv) { return LanguageKind::kObjectivec; }
  if ("objective-cpp" == tv) { return LanguageKind::kObjectivecpp; }
  if ("pascal" == tv) { return LanguageKind::kPascal; }
  if ("perl" == tv) { return LanguageKind::kPerl; }
  if ("perl6" == tv) { return LanguageKind::kPerl6; }
  if ("php" == tv) { return LanguageKind::kPhp; }
  if ("powershell" == tv) { return LanguageKind::kPowershell; }
  if ("jade" == tv) { return LanguageKind::kPug; }
  if ("python" == tv) { return LanguageKind::kPython; }
  if ("r" == tv) { return LanguageKind::kR; }
  if ("razor" == tv) { return LanguageKind::kRazor; }
  if ("ruby" == tv) { return LanguageKind::kRuby; }
  if ("rust" == tv) { return LanguageKind::kRust; }
  if ("scss" == tv) { return LanguageKind::kScss; }
  if ("sass" == tv) { return LanguageKind::kSass; }
  if ("scala" == tv) { return LanguageKind::kScala; }
  if ("shaderlab" == tv) { return LanguageKind::kShaderlab; }
  if ("shellscript" == tv) { return LanguageKind::kShellscript; }
  if ("sql" == tv) { return LanguageKind::kSql; }
  if ("swift" == tv) { return LanguageKind::kSwift; }
  if ("typescript" == tv) { return LanguageKind::kTypescript; }
  if ("typescriptreact" == tv) { return LanguageKind::kTypescriptreact; }
  if ("tex" == tv) { return LanguageKind::kTex; }
  if ("vb" == tv) { return LanguageKind::kVisualbasic; }
  if ("xml" == tv) { return LanguageKind::kXml; }
  if ("xsl" == tv) { return LanguageKind::kXsl; }
  if ("yaml" == tv) { return LanguageKind::kYaml; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(InlineCompletionTriggerKind val, ryml::NodeRef n) {
  switch (val) {
    case InlineCompletionTriggerKind::kInvoked: n = "1"; break;
    case InlineCompletionTriggerKind::kAutomatic: n = "2"; break;
  }
}

template <>
InlineCompletionTriggerKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return InlineCompletionTriggerKind::kInvoked; }
  if ("2" == tv) { return InlineCompletionTriggerKind::kAutomatic; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(PositionEncodingKind val, ryml::NodeRef n) {
  switch (val) {
    case PositionEncodingKind::kUtf8: n = "utf-8"; break;
    case PositionEncodingKind::kUtf16: n = "utf-16"; break;
    case PositionEncodingKind::kUtf32: n = "utf-32"; break;
  }
}

template <>
PositionEncodingKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("utf-8" == tv) { return PositionEncodingKind::kUtf8; }
  if ("utf-16" == tv) { return PositionEncodingKind::kUtf16; }
  if ("utf-32" == tv) { return PositionEncodingKind::kUtf32; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(FileChangeType val, ryml::NodeRef n) {
  switch (val) {
    case FileChangeType::kCreated: n = "1"; break;
    case FileChangeType::kChanged: n = "2"; break;
    case FileChangeType::kDeleted: n = "3"; break;
  }
}

template <>
FileChangeType detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return FileChangeType::kCreated; }
  if ("2" == tv) { return FileChangeType::kChanged; }
  if ("3" == tv) { return FileChangeType::kDeleted; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(WatchKind val, ryml::NodeRef n) {
  switch (val) {
    case WatchKind::kCreate: n = "1"; break;
    case WatchKind::kChange: n = "2"; break;
    case WatchKind::kDelete: n = "4"; break;
  }
}

template <>
WatchKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return WatchKind::kCreate; }
  if ("2" == tv) { return WatchKind::kChange; }
  if ("4" == tv) { return WatchKind::kDelete; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(DiagnosticSeverity val, ryml::NodeRef n) {
  switch (val) {
    case DiagnosticSeverity::kError: n = "1"; break;
    case DiagnosticSeverity::kWarning: n = "2"; break;
    case DiagnosticSeverity::kInformation: n = "3"; break;
    case DiagnosticSeverity::kHint: n = "4"; break;
  }
}

template <>
DiagnosticSeverity detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return DiagnosticSeverity::kError; }
  if ("2" == tv) { return DiagnosticSeverity::kWarning; }
  if ("3" == tv) { return DiagnosticSeverity::kInformation; }
  if ("4" == tv) { return DiagnosticSeverity::kHint; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(DiagnosticTag val, ryml::NodeRef n) {
  switch (val) {
    case DiagnosticTag::kUnnecessary: n = "1"; break;
    case DiagnosticTag::kDeprecated: n = "2"; break;
  }
}

template <>
DiagnosticTag detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return DiagnosticTag::kUnnecessary; }
  if ("2" == tv) { return DiagnosticTag::kDeprecated; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(CompletionTriggerKind val, ryml::NodeRef n) {
  switch (val) {
    case CompletionTriggerKind::kInvoked: n = "1"; break;
    case CompletionTriggerKind::kTriggercharacter: n = "2"; break;
    case CompletionTriggerKind::kTriggerforincompletecompletions: n = "3"; break;
  }
}

template <>
CompletionTriggerKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return CompletionTriggerKind::kInvoked; }
  if ("2" == tv) { return CompletionTriggerKind::kTriggercharacter; }
  if ("3" == tv) { return CompletionTriggerKind::kTriggerforincompletecompletions; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(ApplyKind val, ryml::NodeRef n) {
  switch (val) {
    case ApplyKind::kReplace: n = "1"; break;
    case ApplyKind::kMerge: n = "2"; break;
  }
}

template <>
ApplyKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return ApplyKind::kReplace; }
  if ("2" == tv) { return ApplyKind::kMerge; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(SignatureHelpTriggerKind val, ryml::NodeRef n) {
  switch (val) {
    case SignatureHelpTriggerKind::kInvoked: n = "1"; break;
    case SignatureHelpTriggerKind::kTriggercharacter: n = "2"; break;
    case SignatureHelpTriggerKind::kContentchange: n = "3"; break;
  }
}

template <>
SignatureHelpTriggerKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return SignatureHelpTriggerKind::kInvoked; }
  if ("2" == tv) { return SignatureHelpTriggerKind::kTriggercharacter; }
  if ("3" == tv) { return SignatureHelpTriggerKind::kContentchange; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(CodeActionTriggerKind val, ryml::NodeRef n) {
  switch (val) {
    case CodeActionTriggerKind::kInvoked: n = "1"; break;
    case CodeActionTriggerKind::kAutomatic: n = "2"; break;
  }
}

template <>
CodeActionTriggerKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return CodeActionTriggerKind::kInvoked; }
  if ("2" == tv) { return CodeActionTriggerKind::kAutomatic; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(FileOperationPatternKind val, ryml::NodeRef n) {
  switch (val) {
    case FileOperationPatternKind::kFile: n = "file"; break;
    case FileOperationPatternKind::kFolder: n = "folder"; break;
  }
}

template <>
FileOperationPatternKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("file" == tv) { return FileOperationPatternKind::kFile; }
  if ("folder" == tv) { return FileOperationPatternKind::kFolder; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(NotebookCellKind val, ryml::NodeRef n) {
  switch (val) {
    case NotebookCellKind::kMarkup: n = "1"; break;
    case NotebookCellKind::kCode: n = "2"; break;
  }
}

template <>
NotebookCellKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return NotebookCellKind::kMarkup; }
  if ("2" == tv) { return NotebookCellKind::kCode; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(ResourceOperationKind val, ryml::NodeRef n) {
  switch (val) {
    case ResourceOperationKind::kCreate: n = "create"; break;
    case ResourceOperationKind::kRename: n = "rename"; break;
    case ResourceOperationKind::kDelete: n = "delete"; break;
  }
}

template <>
ResourceOperationKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("create" == tv) { return ResourceOperationKind::kCreate; }
  if ("rename" == tv) { return ResourceOperationKind::kRename; }
  if ("delete" == tv) { return ResourceOperationKind::kDelete; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(FailureHandlingKind val, ryml::NodeRef n) {
  switch (val) {
    case FailureHandlingKind::kAbort: n = "abort"; break;
    case FailureHandlingKind::kTransactional: n = "transactional"; break;
    case FailureHandlingKind::kTextonlytransactional: n = "textOnlyTransactional"; break;
    case FailureHandlingKind::kUndo: n = "undo"; break;
  }
}

template <>
FailureHandlingKind detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("abort" == tv) { return FailureHandlingKind::kAbort; }
  if ("transactional" == tv) { return FailureHandlingKind::kTransactional; }
  if ("textOnlyTransactional" == tv) { return FailureHandlingKind::kTextonlytransactional; }
  if ("undo" == tv) { return FailureHandlingKind::kUndo; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(PrepareSupportDefaultBehavior val, ryml::NodeRef n) {
  switch (val) {
    case PrepareSupportDefaultBehavior::kIdentifier: n = "1"; break;
  }
}

template <>
PrepareSupportDefaultBehavior detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("1" == tv) { return PrepareSupportDefaultBehavior::kIdentifier; }
  assert(false);
  return {};
}

template <>
void detail::SerializeEnum(TokenFormat val, ryml::NodeRef n) {
  switch (val) {
    case TokenFormat::kRelative: n = "relative"; break;
  }
}

template <>
TokenFormat detail::DeserializeEnum(ryml::ConstNodeRef n) {
  const auto tv = n.val();
  if ("relative" == tv) { return TokenFormat::kRelative; }
  assert(false);
  return {};
}

TextDocumentIdentifier ImplementationParams::textDocument() { return n_.find_child("textDocument"); }
Position ImplementationParams::position() { return n_.find_child("position"); }
bool ImplementationParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t ImplementationParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void ImplementationParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool ImplementationParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view ImplementationParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void ImplementationParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
ImplementationParams::Union2 ImplementationParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool ImplementationParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
ImplementationParams::Union2 ImplementationParams::add_workDoneToken() { auto w = ImplementationParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool ImplementationParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t ImplementationParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void ImplementationParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool ImplementationParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view ImplementationParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void ImplementationParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
ImplementationParams::Union3 ImplementationParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool ImplementationParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
ImplementationParams::Union3 ImplementationParams::add_partialResultToken() { auto w = ImplementationParams::Union3(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void ImplementationParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
std::string_view Location::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void Location::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
Range Location::range() { return n_.find_child("range"); }
void Location::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); }
bool ImplementationRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector ImplementationRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
ImplementationRegistrationOptions::Union0 ImplementationRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
std::string_view ImplementationRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool ImplementationRegistrationOptions::has_id() const { return n_.has_child("id"); }
void ImplementationRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void ImplementationRegistrationOptions::init() { StructWrapper::init(); ImplementationRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier TypeDefinitionParams::textDocument() { return n_.find_child("textDocument"); }
Position TypeDefinitionParams::position() { return n_.find_child("position"); }
bool TypeDefinitionParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t TypeDefinitionParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void TypeDefinitionParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool TypeDefinitionParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view TypeDefinitionParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void TypeDefinitionParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
TypeDefinitionParams::Union2 TypeDefinitionParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool TypeDefinitionParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
TypeDefinitionParams::Union2 TypeDefinitionParams::add_workDoneToken() { auto w = TypeDefinitionParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool TypeDefinitionParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t TypeDefinitionParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void TypeDefinitionParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool TypeDefinitionParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view TypeDefinitionParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void TypeDefinitionParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
TypeDefinitionParams::Union3 TypeDefinitionParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool TypeDefinitionParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
TypeDefinitionParams::Union3 TypeDefinitionParams::add_partialResultToken() { auto w = TypeDefinitionParams::Union3(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void TypeDefinitionParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
bool TypeDefinitionRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector TypeDefinitionRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
TypeDefinitionRegistrationOptions::Union0 TypeDefinitionRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
std::string_view TypeDefinitionRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool TypeDefinitionRegistrationOptions::has_id() const { return n_.has_child("id"); }
void TypeDefinitionRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void TypeDefinitionRegistrationOptions::init() { StructWrapper::init(); TypeDefinitionRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
std::string_view WorkspaceFolder::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void WorkspaceFolder::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
std::string_view WorkspaceFolder::name() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("name")); }
void WorkspaceFolder::set_name(std::string_view val) { auto c = n_.find_child("name"); if (c.invalid()) { c = n_.append_child() << ryml::key("name"); } detail::SetPrimitive(c, val); }
void WorkspaceFolder::init() { StructWrapper::init();  }
WorkspaceFoldersChangeEvent DidChangeWorkspaceFoldersParams::event() { return n_.find_child("event"); }
void DidChangeWorkspaceFoldersParams::init() { StructWrapper::init(); WorkspaceFoldersChangeEvent(n_.append_child() << ryml::key("event")).init(); }
List<ConfigurationItem> ConfigurationParams::items() { return n_.find_child("items"); }
void ConfigurationParams::init() { StructWrapper::init(); List<ConfigurationItem>(n_.append_child() << ryml::key("items")).init(); }
TextDocumentIdentifier DocumentColorParams::textDocument() { return n_.find_child("textDocument"); }
bool DocumentColorParams::Union1::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DocumentColorParams::Union1::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DocumentColorParams::Union1::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DocumentColorParams::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view DocumentColorParams::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DocumentColorParams::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DocumentColorParams::Union1 DocumentColorParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool DocumentColorParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
DocumentColorParams::Union1 DocumentColorParams::add_workDoneToken() { auto w = DocumentColorParams::Union1(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool DocumentColorParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DocumentColorParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DocumentColorParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DocumentColorParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view DocumentColorParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DocumentColorParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DocumentColorParams::Union2 DocumentColorParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool DocumentColorParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
DocumentColorParams::Union2 DocumentColorParams::add_partialResultToken() { auto w = DocumentColorParams::Union2(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void DocumentColorParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); }
Range ColorInformation::range() { return n_.find_child("range"); }
Color ColorInformation::color() { return n_.find_child("color"); }
void ColorInformation::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); Color(n_.append_child() << ryml::key("color")).init(); }
bool DocumentColorRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector DocumentColorRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
DocumentColorRegistrationOptions::Union0 DocumentColorRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
std::string_view DocumentColorRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool DocumentColorRegistrationOptions::has_id() const { return n_.has_child("id"); }
void DocumentColorRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void DocumentColorRegistrationOptions::init() { StructWrapper::init(); DocumentColorRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier ColorPresentationParams::textDocument() { return n_.find_child("textDocument"); }
Color ColorPresentationParams::color() { return n_.find_child("color"); }
Range ColorPresentationParams::range() { return n_.find_child("range"); }
bool ColorPresentationParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t ColorPresentationParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void ColorPresentationParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool ColorPresentationParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view ColorPresentationParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void ColorPresentationParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
ColorPresentationParams::Union3 ColorPresentationParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool ColorPresentationParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
ColorPresentationParams::Union3 ColorPresentationParams::add_workDoneToken() { auto w = ColorPresentationParams::Union3(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool ColorPresentationParams::Union4::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t ColorPresentationParams::Union4::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void ColorPresentationParams::Union4::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool ColorPresentationParams::Union4::holds_string() const { return n_.has_child("TODO"); }
std::string_view ColorPresentationParams::Union4::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void ColorPresentationParams::Union4::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
ColorPresentationParams::Union4 ColorPresentationParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool ColorPresentationParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
ColorPresentationParams::Union4 ColorPresentationParams::add_partialResultToken() { auto w = ColorPresentationParams::Union4(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void ColorPresentationParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Color(n_.append_child() << ryml::key("color")).init(); Range(n_.append_child() << ryml::key("range")).init(); }
std::string_view ColorPresentation::label() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("label")); }
void ColorPresentation::set_label(std::string_view val) { auto c = n_.find_child("label"); if (c.invalid()) { c = n_.append_child() << ryml::key("label"); } detail::SetPrimitive(c, val); }
TextEdit ColorPresentation::textEdit() { return n_.find_child("textEdit"); }
bool ColorPresentation::has_textEdit() const { return n_.has_child("textEdit"); }
TextEdit ColorPresentation::add_textEdit() { auto w = TextEdit(n_.append_child() << ryml::key("textEdit")); w.init(); return w; }
List<TextEdit> ColorPresentation::additionalTextEdits() { return n_.find_child("additionalTextEdits"); }
bool ColorPresentation::has_additionalTextEdits() const { return n_.has_child("additionalTextEdits"); }
List<TextEdit> ColorPresentation::add_additionalTextEdits() { auto w = List<TextEdit>(n_.append_child() << ryml::key("additionalTextEdits")); w.init(); return w; }
void ColorPresentation::init() { StructWrapper::init();  }
bool WorkDoneProgressOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool WorkDoneProgressOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void WorkDoneProgressOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void WorkDoneProgressOptions::init() { StructWrapper::init();  }
bool TextDocumentRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector TextDocumentRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
TextDocumentRegistrationOptions::Union0 TextDocumentRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
void TextDocumentRegistrationOptions::init() { StructWrapper::init(); TextDocumentRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier FoldingRangeParams::textDocument() { return n_.find_child("textDocument"); }
bool FoldingRangeParams::Union1::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t FoldingRangeParams::Union1::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void FoldingRangeParams::Union1::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool FoldingRangeParams::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view FoldingRangeParams::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void FoldingRangeParams::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
FoldingRangeParams::Union1 FoldingRangeParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool FoldingRangeParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
FoldingRangeParams::Union1 FoldingRangeParams::add_workDoneToken() { auto w = FoldingRangeParams::Union1(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool FoldingRangeParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t FoldingRangeParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void FoldingRangeParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool FoldingRangeParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view FoldingRangeParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void FoldingRangeParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
FoldingRangeParams::Union2 FoldingRangeParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool FoldingRangeParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
FoldingRangeParams::Union2 FoldingRangeParams::add_partialResultToken() { auto w = FoldingRangeParams::Union2(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void FoldingRangeParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); }
std::uint32_t FoldingRange::startLine() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("startLine")); }
void FoldingRange::set_startLine(std::uint32_t val) { auto c = n_.find_child("startLine"); if (c.invalid()) { c = n_.append_child() << ryml::key("startLine"); } detail::SetPrimitive(c, val); }
std::uint32_t FoldingRange::startCharacter() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("startCharacter")); }
bool FoldingRange::has_startCharacter() const { return n_.has_child("startCharacter"); }
void FoldingRange::set_startCharacter(std::uint32_t val) { auto c = n_.find_child("startCharacter"); if (c.invalid()) { c = n_.append_child() << ryml::key("startCharacter"); } detail::SetPrimitive(c, val); }
std::uint32_t FoldingRange::endLine() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("endLine")); }
void FoldingRange::set_endLine(std::uint32_t val) { auto c = n_.find_child("endLine"); if (c.invalid()) { c = n_.append_child() << ryml::key("endLine"); } detail::SetPrimitive(c, val); }
std::uint32_t FoldingRange::endCharacter() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("endCharacter")); }
bool FoldingRange::has_endCharacter() const { return n_.has_child("endCharacter"); }
void FoldingRange::set_endCharacter(std::uint32_t val) { auto c = n_.find_child("endCharacter"); if (c.invalid()) { c = n_.append_child() << ryml::key("endCharacter"); } detail::SetPrimitive(c, val); }
FoldingRangeKind FoldingRange::kind() { return detail::DeserializeEnum<FoldingRangeKind>(n_.find_child("kind")); }
bool FoldingRange::has_kind() const { return n_.has_child("kind"); }
void FoldingRange::set_kind(FoldingRangeKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
std::string_view FoldingRange::collapsedText() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("collapsedText")); }
bool FoldingRange::has_collapsedText() const { return n_.has_child("collapsedText"); }
void FoldingRange::set_collapsedText(std::string_view val) { auto c = n_.find_child("collapsedText"); if (c.invalid()) { c = n_.append_child() << ryml::key("collapsedText"); } detail::SetPrimitive(c, val); }
void FoldingRange::init() { StructWrapper::init();  }
bool FoldingRangeRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector FoldingRangeRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
FoldingRangeRegistrationOptions::Union0 FoldingRangeRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
std::string_view FoldingRangeRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool FoldingRangeRegistrationOptions::has_id() const { return n_.has_child("id"); }
void FoldingRangeRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void FoldingRangeRegistrationOptions::init() { StructWrapper::init(); FoldingRangeRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier DeclarationParams::textDocument() { return n_.find_child("textDocument"); }
Position DeclarationParams::position() { return n_.find_child("position"); }
bool DeclarationParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DeclarationParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DeclarationParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DeclarationParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view DeclarationParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DeclarationParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DeclarationParams::Union2 DeclarationParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool DeclarationParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
DeclarationParams::Union2 DeclarationParams::add_workDoneToken() { auto w = DeclarationParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool DeclarationParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DeclarationParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DeclarationParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DeclarationParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view DeclarationParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DeclarationParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DeclarationParams::Union3 DeclarationParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool DeclarationParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
DeclarationParams::Union3 DeclarationParams::add_partialResultToken() { auto w = DeclarationParams::Union3(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void DeclarationParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
bool DeclarationRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector DeclarationRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
DeclarationRegistrationOptions::Union0 DeclarationRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
std::string_view DeclarationRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool DeclarationRegistrationOptions::has_id() const { return n_.has_child("id"); }
void DeclarationRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void DeclarationRegistrationOptions::init() { StructWrapper::init(); DeclarationRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier SelectionRangeParams::textDocument() { return n_.find_child("textDocument"); }
List<Position> SelectionRangeParams::positions() { return n_.find_child("positions"); }
bool SelectionRangeParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t SelectionRangeParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void SelectionRangeParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool SelectionRangeParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view SelectionRangeParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void SelectionRangeParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
SelectionRangeParams::Union2 SelectionRangeParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool SelectionRangeParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
SelectionRangeParams::Union2 SelectionRangeParams::add_workDoneToken() { auto w = SelectionRangeParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool SelectionRangeParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t SelectionRangeParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void SelectionRangeParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool SelectionRangeParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view SelectionRangeParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void SelectionRangeParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
SelectionRangeParams::Union3 SelectionRangeParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool SelectionRangeParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
SelectionRangeParams::Union3 SelectionRangeParams::add_partialResultToken() { auto w = SelectionRangeParams::Union3(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void SelectionRangeParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); List<Position>(n_.append_child() << ryml::key("positions")).init(); }
Range SelectionRange::range() { return n_.find_child("range"); }
SelectionRange SelectionRange::parent() { return n_.find_child("parent"); }
bool SelectionRange::has_parent() const { return n_.has_child("parent"); }
SelectionRange SelectionRange::add_parent() { auto w = SelectionRange(n_.append_child() << ryml::key("parent")); w.init(); return w; }
void SelectionRange::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); }
bool SelectionRangeRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector SelectionRangeRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
SelectionRangeRegistrationOptions::Union0 SelectionRangeRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
std::string_view SelectionRangeRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool SelectionRangeRegistrationOptions::has_id() const { return n_.has_child("id"); }
void SelectionRangeRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void SelectionRangeRegistrationOptions::init() { StructWrapper::init(); SelectionRangeRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
bool WorkDoneProgressCreateParams::Union0::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t WorkDoneProgressCreateParams::Union0::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void WorkDoneProgressCreateParams::Union0::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool WorkDoneProgressCreateParams::Union0::holds_string() const { return n_.has_child("TODO"); }
std::string_view WorkDoneProgressCreateParams::Union0::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void WorkDoneProgressCreateParams::Union0::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
WorkDoneProgressCreateParams::Union0 WorkDoneProgressCreateParams::token() { return n_.find_child("token"); }
void WorkDoneProgressCreateParams::init() { StructWrapper::init(); WorkDoneProgressCreateParams::Union0(n_.append_child() << ryml::key("token")).init(); }
bool WorkDoneProgressCancelParams::Union0::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t WorkDoneProgressCancelParams::Union0::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void WorkDoneProgressCancelParams::Union0::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool WorkDoneProgressCancelParams::Union0::holds_string() const { return n_.has_child("TODO"); }
std::string_view WorkDoneProgressCancelParams::Union0::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void WorkDoneProgressCancelParams::Union0::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
WorkDoneProgressCancelParams::Union0 WorkDoneProgressCancelParams::token() { return n_.find_child("token"); }
void WorkDoneProgressCancelParams::init() { StructWrapper::init(); WorkDoneProgressCancelParams::Union0(n_.append_child() << ryml::key("token")).init(); }
TextDocumentIdentifier CallHierarchyPrepareParams::textDocument() { return n_.find_child("textDocument"); }
Position CallHierarchyPrepareParams::position() { return n_.find_child("position"); }
bool CallHierarchyPrepareParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CallHierarchyPrepareParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CallHierarchyPrepareParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CallHierarchyPrepareParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view CallHierarchyPrepareParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CallHierarchyPrepareParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
CallHierarchyPrepareParams::Union2 CallHierarchyPrepareParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool CallHierarchyPrepareParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
CallHierarchyPrepareParams::Union2 CallHierarchyPrepareParams::add_workDoneToken() { auto w = CallHierarchyPrepareParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void CallHierarchyPrepareParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
std::string_view CallHierarchyItem::name() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("name")); }
void CallHierarchyItem::set_name(std::string_view val) { auto c = n_.find_child("name"); if (c.invalid()) { c = n_.append_child() << ryml::key("name"); } detail::SetPrimitive(c, val); }
SymbolKind CallHierarchyItem::kind() { return detail::DeserializeEnum<SymbolKind>(n_.find_child("kind")); }
void CallHierarchyItem::set_kind(SymbolKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
List<SymbolTag> CallHierarchyItem::tags() { return n_.find_child("tags"); }
bool CallHierarchyItem::has_tags() const { return n_.has_child("tags"); }
List<SymbolTag> CallHierarchyItem::add_tags() { auto w = List<SymbolTag>(n_.append_child() << ryml::key("tags")); w.init(); return w; }
std::string_view CallHierarchyItem::detail() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("detail")); }
bool CallHierarchyItem::has_detail() const { return n_.has_child("detail"); }
void CallHierarchyItem::set_detail(std::string_view val) { auto c = n_.find_child("detail"); if (c.invalid()) { c = n_.append_child() << ryml::key("detail"); } detail::SetPrimitive(c, val); }
std::string_view CallHierarchyItem::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void CallHierarchyItem::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
Range CallHierarchyItem::range() { return n_.find_child("range"); }
Range CallHierarchyItem::selectionRange() { return n_.find_child("selectionRange"); }
bool CallHierarchyItem::Union7::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject CallHierarchyItem::Union7::as_reference_Map0() {  }
bool CallHierarchyItem::Union7::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray CallHierarchyItem::Union7::as_reference_LSPAny() { return {n_}; }
bool CallHierarchyItem::Union7::holds_string() const { return n_.has_child("TODO"); }
std::string_view CallHierarchyItem::Union7::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CallHierarchyItem::Union7::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool CallHierarchyItem::Union7::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CallHierarchyItem::Union7::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CallHierarchyItem::Union7::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CallHierarchyItem::Union7::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t CallHierarchyItem::Union7::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void CallHierarchyItem::Union7::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool CallHierarchyItem::Union7::holds_decimal() const { return n_.has_child("TODO"); }
double CallHierarchyItem::Union7::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void CallHierarchyItem::Union7::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool CallHierarchyItem::Union7::holds_boolean() const { return n_.has_child("TODO"); }
bool CallHierarchyItem::Union7::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void CallHierarchyItem::Union7::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
CallHierarchyItem::Union7 CallHierarchyItem::data() { return n_.find_child("data"); }
bool CallHierarchyItem::has_data() const { return n_.has_child("data"); }
CallHierarchyItem::Union7 CallHierarchyItem::add_data() { auto w = CallHierarchyItem::Union7(n_.append_child() << ryml::key("data")); w.init(); return w; }
void CallHierarchyItem::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); Range(n_.append_child() << ryml::key("selectionRange")).init(); }
bool CallHierarchyRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector CallHierarchyRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
CallHierarchyRegistrationOptions::Union0 CallHierarchyRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
std::string_view CallHierarchyRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool CallHierarchyRegistrationOptions::has_id() const { return n_.has_child("id"); }
void CallHierarchyRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void CallHierarchyRegistrationOptions::init() { StructWrapper::init(); CallHierarchyRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
CallHierarchyItem CallHierarchyIncomingCallsParams::item() { return n_.find_child("item"); }
bool CallHierarchyIncomingCallsParams::Union1::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CallHierarchyIncomingCallsParams::Union1::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CallHierarchyIncomingCallsParams::Union1::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CallHierarchyIncomingCallsParams::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view CallHierarchyIncomingCallsParams::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CallHierarchyIncomingCallsParams::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
CallHierarchyIncomingCallsParams::Union1 CallHierarchyIncomingCallsParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool CallHierarchyIncomingCallsParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
CallHierarchyIncomingCallsParams::Union1 CallHierarchyIncomingCallsParams::add_workDoneToken() { auto w = CallHierarchyIncomingCallsParams::Union1(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool CallHierarchyIncomingCallsParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CallHierarchyIncomingCallsParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CallHierarchyIncomingCallsParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CallHierarchyIncomingCallsParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view CallHierarchyIncomingCallsParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CallHierarchyIncomingCallsParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
CallHierarchyIncomingCallsParams::Union2 CallHierarchyIncomingCallsParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool CallHierarchyIncomingCallsParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
CallHierarchyIncomingCallsParams::Union2 CallHierarchyIncomingCallsParams::add_partialResultToken() { auto w = CallHierarchyIncomingCallsParams::Union2(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void CallHierarchyIncomingCallsParams::init() { StructWrapper::init(); CallHierarchyItem(n_.append_child() << ryml::key("item")).init(); }
CallHierarchyItem CallHierarchyIncomingCall::from() { return n_.find_child("from"); }
List<Range> CallHierarchyIncomingCall::fromRanges() { return n_.find_child("fromRanges"); }
void CallHierarchyIncomingCall::init() { StructWrapper::init(); CallHierarchyItem(n_.append_child() << ryml::key("from")).init(); List<Range>(n_.append_child() << ryml::key("fromRanges")).init(); }
CallHierarchyItem CallHierarchyOutgoingCallsParams::item() { return n_.find_child("item"); }
bool CallHierarchyOutgoingCallsParams::Union1::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CallHierarchyOutgoingCallsParams::Union1::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CallHierarchyOutgoingCallsParams::Union1::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CallHierarchyOutgoingCallsParams::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view CallHierarchyOutgoingCallsParams::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CallHierarchyOutgoingCallsParams::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
CallHierarchyOutgoingCallsParams::Union1 CallHierarchyOutgoingCallsParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool CallHierarchyOutgoingCallsParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
CallHierarchyOutgoingCallsParams::Union1 CallHierarchyOutgoingCallsParams::add_workDoneToken() { auto w = CallHierarchyOutgoingCallsParams::Union1(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool CallHierarchyOutgoingCallsParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CallHierarchyOutgoingCallsParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CallHierarchyOutgoingCallsParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CallHierarchyOutgoingCallsParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view CallHierarchyOutgoingCallsParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CallHierarchyOutgoingCallsParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
CallHierarchyOutgoingCallsParams::Union2 CallHierarchyOutgoingCallsParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool CallHierarchyOutgoingCallsParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
CallHierarchyOutgoingCallsParams::Union2 CallHierarchyOutgoingCallsParams::add_partialResultToken() { auto w = CallHierarchyOutgoingCallsParams::Union2(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void CallHierarchyOutgoingCallsParams::init() { StructWrapper::init(); CallHierarchyItem(n_.append_child() << ryml::key("item")).init(); }
CallHierarchyItem CallHierarchyOutgoingCall::to() { return n_.find_child("to"); }
List<Range> CallHierarchyOutgoingCall::fromRanges() { return n_.find_child("fromRanges"); }
void CallHierarchyOutgoingCall::init() { StructWrapper::init(); CallHierarchyItem(n_.append_child() << ryml::key("to")).init(); List<Range>(n_.append_child() << ryml::key("fromRanges")).init(); }
TextDocumentIdentifier SemanticTokensParams::textDocument() { return n_.find_child("textDocument"); }
bool SemanticTokensParams::Union1::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t SemanticTokensParams::Union1::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void SemanticTokensParams::Union1::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool SemanticTokensParams::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view SemanticTokensParams::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void SemanticTokensParams::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
SemanticTokensParams::Union1 SemanticTokensParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool SemanticTokensParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
SemanticTokensParams::Union1 SemanticTokensParams::add_workDoneToken() { auto w = SemanticTokensParams::Union1(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool SemanticTokensParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t SemanticTokensParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void SemanticTokensParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool SemanticTokensParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view SemanticTokensParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void SemanticTokensParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
SemanticTokensParams::Union2 SemanticTokensParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool SemanticTokensParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
SemanticTokensParams::Union2 SemanticTokensParams::add_partialResultToken() { auto w = SemanticTokensParams::Union2(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void SemanticTokensParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); }
std::string_view SemanticTokens::resultId() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("resultId")); }
bool SemanticTokens::has_resultId() const { return n_.has_child("resultId"); }
void SemanticTokens::set_resultId(std::string_view val) { auto c = n_.find_child("resultId"); if (c.invalid()) { c = n_.append_child() << ryml::key("resultId"); } detail::SetPrimitive(c, val); }
List<std::uint32_t> SemanticTokens::data() { return n_.find_child("data"); }
void SemanticTokens::init() { StructWrapper::init(); List<std::uint32_t>(n_.append_child() << ryml::key("data")).init(); }
List<std::uint32_t> SemanticTokensPartialResult::data() { return n_.find_child("data"); }
void SemanticTokensPartialResult::init() { StructWrapper::init(); List<std::uint32_t>(n_.append_child() << ryml::key("data")).init(); }
bool SemanticTokensRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector SemanticTokensRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
SemanticTokensRegistrationOptions::Union0 SemanticTokensRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
SemanticTokensLegend SemanticTokensRegistrationOptions::legend() { return n_.find_child("legend"); }
bool SemanticTokensRegistrationOptions::Union2::holds_boolean() const { return n_.has_child("TODO"); }
bool SemanticTokensRegistrationOptions::Union2::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void SemanticTokensRegistrationOptions::Union2::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
SemanticTokensRegistrationOptions::Union2 SemanticTokensRegistrationOptions::range() { return n_.find_child("range"); }
bool SemanticTokensRegistrationOptions::has_range() const { return n_.has_child("range"); }
SemanticTokensRegistrationOptions::Union2 SemanticTokensRegistrationOptions::add_range() { auto w = SemanticTokensRegistrationOptions::Union2(n_.append_child() << ryml::key("range")); w.init(); return w; }
bool SemanticTokensRegistrationOptions::Union3::holds_boolean() const { return n_.has_child("TODO"); }
bool SemanticTokensRegistrationOptions::Union3::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void SemanticTokensRegistrationOptions::Union3::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool SemanticTokensRegistrationOptions::Union3::holds_SemanticTokensFullDelta() const { return n_.has_child("TODO"); }
SemanticTokensFullDelta SemanticTokensRegistrationOptions::Union3::as_SemanticTokensFullDelta() { return {n_}; }
SemanticTokensRegistrationOptions::Union3 SemanticTokensRegistrationOptions::full() { return n_.find_child("full"); }
bool SemanticTokensRegistrationOptions::has_full() const { return n_.has_child("full"); }
SemanticTokensRegistrationOptions::Union3 SemanticTokensRegistrationOptions::add_full() { auto w = SemanticTokensRegistrationOptions::Union3(n_.append_child() << ryml::key("full")); w.init(); return w; }
std::string_view SemanticTokensRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool SemanticTokensRegistrationOptions::has_id() const { return n_.has_child("id"); }
void SemanticTokensRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void SemanticTokensRegistrationOptions::init() { StructWrapper::init(); SemanticTokensRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); SemanticTokensLegend(n_.append_child() << ryml::key("legend")).init(); }
TextDocumentIdentifier SemanticTokensDeltaParams::textDocument() { return n_.find_child("textDocument"); }
std::string_view SemanticTokensDeltaParams::previousResultId() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("previousResultId")); }
void SemanticTokensDeltaParams::set_previousResultId(std::string_view val) { auto c = n_.find_child("previousResultId"); if (c.invalid()) { c = n_.append_child() << ryml::key("previousResultId"); } detail::SetPrimitive(c, val); }
bool SemanticTokensDeltaParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t SemanticTokensDeltaParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void SemanticTokensDeltaParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool SemanticTokensDeltaParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view SemanticTokensDeltaParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void SemanticTokensDeltaParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
SemanticTokensDeltaParams::Union2 SemanticTokensDeltaParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool SemanticTokensDeltaParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
SemanticTokensDeltaParams::Union2 SemanticTokensDeltaParams::add_workDoneToken() { auto w = SemanticTokensDeltaParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool SemanticTokensDeltaParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t SemanticTokensDeltaParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void SemanticTokensDeltaParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool SemanticTokensDeltaParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view SemanticTokensDeltaParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void SemanticTokensDeltaParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
SemanticTokensDeltaParams::Union3 SemanticTokensDeltaParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool SemanticTokensDeltaParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
SemanticTokensDeltaParams::Union3 SemanticTokensDeltaParams::add_partialResultToken() { auto w = SemanticTokensDeltaParams::Union3(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void SemanticTokensDeltaParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); }
std::string_view SemanticTokensDelta::resultId() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("resultId")); }
bool SemanticTokensDelta::has_resultId() const { return n_.has_child("resultId"); }
void SemanticTokensDelta::set_resultId(std::string_view val) { auto c = n_.find_child("resultId"); if (c.invalid()) { c = n_.append_child() << ryml::key("resultId"); } detail::SetPrimitive(c, val); }
List<SemanticTokensEdit> SemanticTokensDelta::edits() { return n_.find_child("edits"); }
void SemanticTokensDelta::init() { StructWrapper::init(); List<SemanticTokensEdit>(n_.append_child() << ryml::key("edits")).init(); }
List<SemanticTokensEdit> SemanticTokensDeltaPartialResult::edits() { return n_.find_child("edits"); }
void SemanticTokensDeltaPartialResult::init() { StructWrapper::init(); List<SemanticTokensEdit>(n_.append_child() << ryml::key("edits")).init(); }
TextDocumentIdentifier SemanticTokensRangeParams::textDocument() { return n_.find_child("textDocument"); }
Range SemanticTokensRangeParams::range() { return n_.find_child("range"); }
bool SemanticTokensRangeParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t SemanticTokensRangeParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void SemanticTokensRangeParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool SemanticTokensRangeParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view SemanticTokensRangeParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void SemanticTokensRangeParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
SemanticTokensRangeParams::Union2 SemanticTokensRangeParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool SemanticTokensRangeParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
SemanticTokensRangeParams::Union2 SemanticTokensRangeParams::add_workDoneToken() { auto w = SemanticTokensRangeParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool SemanticTokensRangeParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t SemanticTokensRangeParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void SemanticTokensRangeParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool SemanticTokensRangeParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view SemanticTokensRangeParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void SemanticTokensRangeParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
SemanticTokensRangeParams::Union3 SemanticTokensRangeParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool SemanticTokensRangeParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
SemanticTokensRangeParams::Union3 SemanticTokensRangeParams::add_partialResultToken() { auto w = SemanticTokensRangeParams::Union3(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void SemanticTokensRangeParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Range(n_.append_child() << ryml::key("range")).init(); }
std::string_view ShowDocumentParams::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void ShowDocumentParams::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
bool ShowDocumentParams::external() { return detail::NodeAsPrimitive<bool>(n_.find_child("external")); }
bool ShowDocumentParams::has_external() const { return n_.has_child("external"); }
void ShowDocumentParams::set_external(bool val) { auto c = n_.find_child("external"); if (c.invalid()) { c = n_.append_child() << ryml::key("external"); } detail::SetPrimitive(c, val); }
bool ShowDocumentParams::takeFocus() { return detail::NodeAsPrimitive<bool>(n_.find_child("takeFocus")); }
bool ShowDocumentParams::has_takeFocus() const { return n_.has_child("takeFocus"); }
void ShowDocumentParams::set_takeFocus(bool val) { auto c = n_.find_child("takeFocus"); if (c.invalid()) { c = n_.append_child() << ryml::key("takeFocus"); } detail::SetPrimitive(c, val); }
Range ShowDocumentParams::selection() { return n_.find_child("selection"); }
bool ShowDocumentParams::has_selection() const { return n_.has_child("selection"); }
Range ShowDocumentParams::add_selection() { auto w = Range(n_.append_child() << ryml::key("selection")); w.init(); return w; }
void ShowDocumentParams::init() { StructWrapper::init();  }
bool ShowDocumentResult::success() { return detail::NodeAsPrimitive<bool>(n_.find_child("success")); }
void ShowDocumentResult::set_success(bool val) { auto c = n_.find_child("success"); if (c.invalid()) { c = n_.append_child() << ryml::key("success"); } detail::SetPrimitive(c, val); }
void ShowDocumentResult::init() { StructWrapper::init();  }
TextDocumentIdentifier LinkedEditingRangeParams::textDocument() { return n_.find_child("textDocument"); }
Position LinkedEditingRangeParams::position() { return n_.find_child("position"); }
bool LinkedEditingRangeParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t LinkedEditingRangeParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void LinkedEditingRangeParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool LinkedEditingRangeParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view LinkedEditingRangeParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void LinkedEditingRangeParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
LinkedEditingRangeParams::Union2 LinkedEditingRangeParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool LinkedEditingRangeParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
LinkedEditingRangeParams::Union2 LinkedEditingRangeParams::add_workDoneToken() { auto w = LinkedEditingRangeParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void LinkedEditingRangeParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
List<Range> LinkedEditingRanges::ranges() { return n_.find_child("ranges"); }
std::string_view LinkedEditingRanges::wordPattern() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("wordPattern")); }
bool LinkedEditingRanges::has_wordPattern() const { return n_.has_child("wordPattern"); }
void LinkedEditingRanges::set_wordPattern(std::string_view val) { auto c = n_.find_child("wordPattern"); if (c.invalid()) { c = n_.append_child() << ryml::key("wordPattern"); } detail::SetPrimitive(c, val); }
void LinkedEditingRanges::init() { StructWrapper::init(); List<Range>(n_.append_child() << ryml::key("ranges")).init(); }
bool LinkedEditingRangeRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector LinkedEditingRangeRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
LinkedEditingRangeRegistrationOptions::Union0 LinkedEditingRangeRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
std::string_view LinkedEditingRangeRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool LinkedEditingRangeRegistrationOptions::has_id() const { return n_.has_child("id"); }
void LinkedEditingRangeRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void LinkedEditingRangeRegistrationOptions::init() { StructWrapper::init(); LinkedEditingRangeRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
List<FileCreate> CreateFilesParams::files() { return n_.find_child("files"); }
void CreateFilesParams::init() { StructWrapper::init(); List<FileCreate>(n_.append_child() << ryml::key("files")).init(); }
Dict<std::string_view, List<TextEdit>> WorkspaceEdit::changes() { return n_.find_child("changes"); }
bool WorkspaceEdit::has_changes() const { return n_.has_child("changes"); }
Dict<std::string_view, List<TextEdit>> WorkspaceEdit::add_changes() { auto w = Dict<std::string_view, List<TextEdit>>(n_.append_child() << ryml::key("changes")); w.init(); return w; }
List<std::variant<TextDocumentEdit, CreateFile, RenameFile, DeleteFile>> WorkspaceEdit::documentChanges() { return n_.find_child("documentChanges"); }
bool WorkspaceEdit::has_documentChanges() const { return n_.has_child("documentChanges"); }
List<std::variant<TextDocumentEdit, CreateFile, RenameFile, DeleteFile>> WorkspaceEdit::add_documentChanges() { auto w = List<std::variant<TextDocumentEdit, CreateFile, RenameFile, DeleteFile>>(n_.append_child() << ryml::key("documentChanges")); w.init(); return w; }
Dict<ChangeAnnotationIdentifier, ChangeAnnotation> WorkspaceEdit::changeAnnotations() { return n_.find_child("changeAnnotations"); }
bool WorkspaceEdit::has_changeAnnotations() const { return n_.has_child("changeAnnotations"); }
Dict<ChangeAnnotationIdentifier, ChangeAnnotation> WorkspaceEdit::add_changeAnnotations() { auto w = Dict<ChangeAnnotationIdentifier, ChangeAnnotation>(n_.append_child() << ryml::key("changeAnnotations")); w.init(); return w; }
void WorkspaceEdit::init() { StructWrapper::init();  }
List<FileOperationFilter> FileOperationRegistrationOptions::filters() { return n_.find_child("filters"); }
void FileOperationRegistrationOptions::init() { StructWrapper::init(); List<FileOperationFilter>(n_.append_child() << ryml::key("filters")).init(); }
List<FileRename> RenameFilesParams::files() { return n_.find_child("files"); }
void RenameFilesParams::init() { StructWrapper::init(); List<FileRename>(n_.append_child() << ryml::key("files")).init(); }
List<FileDelete> DeleteFilesParams::files() { return n_.find_child("files"); }
void DeleteFilesParams::init() { StructWrapper::init(); List<FileDelete>(n_.append_child() << ryml::key("files")).init(); }
TextDocumentIdentifier MonikerParams::textDocument() { return n_.find_child("textDocument"); }
Position MonikerParams::position() { return n_.find_child("position"); }
bool MonikerParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t MonikerParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void MonikerParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool MonikerParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view MonikerParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void MonikerParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
MonikerParams::Union2 MonikerParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool MonikerParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
MonikerParams::Union2 MonikerParams::add_workDoneToken() { auto w = MonikerParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool MonikerParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t MonikerParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void MonikerParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool MonikerParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view MonikerParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void MonikerParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
MonikerParams::Union3 MonikerParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool MonikerParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
MonikerParams::Union3 MonikerParams::add_partialResultToken() { auto w = MonikerParams::Union3(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void MonikerParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
std::string_view Moniker::scheme() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("scheme")); }
void Moniker::set_scheme(std::string_view val) { auto c = n_.find_child("scheme"); if (c.invalid()) { c = n_.append_child() << ryml::key("scheme"); } detail::SetPrimitive(c, val); }
std::string_view Moniker::identifier() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("identifier")); }
void Moniker::set_identifier(std::string_view val) { auto c = n_.find_child("identifier"); if (c.invalid()) { c = n_.append_child() << ryml::key("identifier"); } detail::SetPrimitive(c, val); }
UniquenessLevel Moniker::unique() { return detail::DeserializeEnum<UniquenessLevel>(n_.find_child("unique")); }
void Moniker::set_unique(UniquenessLevel val) { auto c = n_.find_child("unique"); if (c.invalid()) { c = n_.append_child() << ryml::key("unique"); } detail::SerializeEnum(val, c); }
MonikerKind Moniker::kind() { return detail::DeserializeEnum<MonikerKind>(n_.find_child("kind")); }
bool Moniker::has_kind() const { return n_.has_child("kind"); }
void Moniker::set_kind(MonikerKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
void Moniker::init() { StructWrapper::init();  }
bool MonikerRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector MonikerRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
MonikerRegistrationOptions::Union0 MonikerRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
void MonikerRegistrationOptions::init() { StructWrapper::init(); MonikerRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier TypeHierarchyPrepareParams::textDocument() { return n_.find_child("textDocument"); }
Position TypeHierarchyPrepareParams::position() { return n_.find_child("position"); }
bool TypeHierarchyPrepareParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t TypeHierarchyPrepareParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void TypeHierarchyPrepareParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool TypeHierarchyPrepareParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view TypeHierarchyPrepareParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void TypeHierarchyPrepareParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
TypeHierarchyPrepareParams::Union2 TypeHierarchyPrepareParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool TypeHierarchyPrepareParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
TypeHierarchyPrepareParams::Union2 TypeHierarchyPrepareParams::add_workDoneToken() { auto w = TypeHierarchyPrepareParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void TypeHierarchyPrepareParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
std::string_view TypeHierarchyItem::name() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("name")); }
void TypeHierarchyItem::set_name(std::string_view val) { auto c = n_.find_child("name"); if (c.invalid()) { c = n_.append_child() << ryml::key("name"); } detail::SetPrimitive(c, val); }
SymbolKind TypeHierarchyItem::kind() { return detail::DeserializeEnum<SymbolKind>(n_.find_child("kind")); }
void TypeHierarchyItem::set_kind(SymbolKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
List<SymbolTag> TypeHierarchyItem::tags() { return n_.find_child("tags"); }
bool TypeHierarchyItem::has_tags() const { return n_.has_child("tags"); }
List<SymbolTag> TypeHierarchyItem::add_tags() { auto w = List<SymbolTag>(n_.append_child() << ryml::key("tags")); w.init(); return w; }
std::string_view TypeHierarchyItem::detail() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("detail")); }
bool TypeHierarchyItem::has_detail() const { return n_.has_child("detail"); }
void TypeHierarchyItem::set_detail(std::string_view val) { auto c = n_.find_child("detail"); if (c.invalid()) { c = n_.append_child() << ryml::key("detail"); } detail::SetPrimitive(c, val); }
std::string_view TypeHierarchyItem::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void TypeHierarchyItem::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
Range TypeHierarchyItem::range() { return n_.find_child("range"); }
Range TypeHierarchyItem::selectionRange() { return n_.find_child("selectionRange"); }
bool TypeHierarchyItem::Union7::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject TypeHierarchyItem::Union7::as_reference_Map0() {  }
bool TypeHierarchyItem::Union7::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray TypeHierarchyItem::Union7::as_reference_LSPAny() { return {n_}; }
bool TypeHierarchyItem::Union7::holds_string() const { return n_.has_child("TODO"); }
std::string_view TypeHierarchyItem::Union7::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void TypeHierarchyItem::Union7::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool TypeHierarchyItem::Union7::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t TypeHierarchyItem::Union7::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void TypeHierarchyItem::Union7::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool TypeHierarchyItem::Union7::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t TypeHierarchyItem::Union7::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void TypeHierarchyItem::Union7::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool TypeHierarchyItem::Union7::holds_decimal() const { return n_.has_child("TODO"); }
double TypeHierarchyItem::Union7::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void TypeHierarchyItem::Union7::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool TypeHierarchyItem::Union7::holds_boolean() const { return n_.has_child("TODO"); }
bool TypeHierarchyItem::Union7::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void TypeHierarchyItem::Union7::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
TypeHierarchyItem::Union7 TypeHierarchyItem::data() { return n_.find_child("data"); }
bool TypeHierarchyItem::has_data() const { return n_.has_child("data"); }
TypeHierarchyItem::Union7 TypeHierarchyItem::add_data() { auto w = TypeHierarchyItem::Union7(n_.append_child() << ryml::key("data")); w.init(); return w; }
void TypeHierarchyItem::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); Range(n_.append_child() << ryml::key("selectionRange")).init(); }
bool TypeHierarchyRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector TypeHierarchyRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
TypeHierarchyRegistrationOptions::Union0 TypeHierarchyRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
std::string_view TypeHierarchyRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool TypeHierarchyRegistrationOptions::has_id() const { return n_.has_child("id"); }
void TypeHierarchyRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void TypeHierarchyRegistrationOptions::init() { StructWrapper::init(); TypeHierarchyRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TypeHierarchyItem TypeHierarchySupertypesParams::item() { return n_.find_child("item"); }
bool TypeHierarchySupertypesParams::Union1::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t TypeHierarchySupertypesParams::Union1::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void TypeHierarchySupertypesParams::Union1::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool TypeHierarchySupertypesParams::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view TypeHierarchySupertypesParams::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void TypeHierarchySupertypesParams::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
TypeHierarchySupertypesParams::Union1 TypeHierarchySupertypesParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool TypeHierarchySupertypesParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
TypeHierarchySupertypesParams::Union1 TypeHierarchySupertypesParams::add_workDoneToken() { auto w = TypeHierarchySupertypesParams::Union1(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool TypeHierarchySupertypesParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t TypeHierarchySupertypesParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void TypeHierarchySupertypesParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool TypeHierarchySupertypesParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view TypeHierarchySupertypesParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void TypeHierarchySupertypesParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
TypeHierarchySupertypesParams::Union2 TypeHierarchySupertypesParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool TypeHierarchySupertypesParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
TypeHierarchySupertypesParams::Union2 TypeHierarchySupertypesParams::add_partialResultToken() { auto w = TypeHierarchySupertypesParams::Union2(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void TypeHierarchySupertypesParams::init() { StructWrapper::init(); TypeHierarchyItem(n_.append_child() << ryml::key("item")).init(); }
TypeHierarchyItem TypeHierarchySubtypesParams::item() { return n_.find_child("item"); }
bool TypeHierarchySubtypesParams::Union1::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t TypeHierarchySubtypesParams::Union1::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void TypeHierarchySubtypesParams::Union1::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool TypeHierarchySubtypesParams::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view TypeHierarchySubtypesParams::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void TypeHierarchySubtypesParams::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
TypeHierarchySubtypesParams::Union1 TypeHierarchySubtypesParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool TypeHierarchySubtypesParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
TypeHierarchySubtypesParams::Union1 TypeHierarchySubtypesParams::add_workDoneToken() { auto w = TypeHierarchySubtypesParams::Union1(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool TypeHierarchySubtypesParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t TypeHierarchySubtypesParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void TypeHierarchySubtypesParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool TypeHierarchySubtypesParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view TypeHierarchySubtypesParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void TypeHierarchySubtypesParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
TypeHierarchySubtypesParams::Union2 TypeHierarchySubtypesParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool TypeHierarchySubtypesParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
TypeHierarchySubtypesParams::Union2 TypeHierarchySubtypesParams::add_partialResultToken() { auto w = TypeHierarchySubtypesParams::Union2(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void TypeHierarchySubtypesParams::init() { StructWrapper::init(); TypeHierarchyItem(n_.append_child() << ryml::key("item")).init(); }
TextDocumentIdentifier InlineValueParams::textDocument() { return n_.find_child("textDocument"); }
Range InlineValueParams::range() { return n_.find_child("range"); }
InlineValueContext InlineValueParams::context() { return n_.find_child("context"); }
bool InlineValueParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t InlineValueParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void InlineValueParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool InlineValueParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view InlineValueParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void InlineValueParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
InlineValueParams::Union3 InlineValueParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool InlineValueParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
InlineValueParams::Union3 InlineValueParams::add_workDoneToken() { auto w = InlineValueParams::Union3(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void InlineValueParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Range(n_.append_child() << ryml::key("range")).init(); InlineValueContext(n_.append_child() << ryml::key("context")).init(); }
bool InlineValueRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector InlineValueRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
InlineValueRegistrationOptions::Union0 InlineValueRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
std::string_view InlineValueRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool InlineValueRegistrationOptions::has_id() const { return n_.has_child("id"); }
void InlineValueRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void InlineValueRegistrationOptions::init() { StructWrapper::init(); InlineValueRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier InlayHintParams::textDocument() { return n_.find_child("textDocument"); }
Range InlayHintParams::range() { return n_.find_child("range"); }
bool InlayHintParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t InlayHintParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void InlayHintParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool InlayHintParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view InlayHintParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void InlayHintParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
InlayHintParams::Union2 InlayHintParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool InlayHintParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
InlayHintParams::Union2 InlayHintParams::add_workDoneToken() { auto w = InlayHintParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void InlayHintParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Range(n_.append_child() << ryml::key("range")).init(); }
Position InlayHint::position() { return n_.find_child("position"); }
bool InlayHint::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view InlayHint::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void InlayHint::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool InlayHint::Union1::holds_array_InlayHintLabelPart() const { return n_.has_child("TODO"); }
List<InlayHintLabelPart> InlayHint::Union1::as_array_InlayHintLabelPart() { return {n_}; }
InlayHint::Union1 InlayHint::label() { return n_.find_child("label"); }
InlayHintKind InlayHint::kind() { return detail::DeserializeEnum<InlayHintKind>(n_.find_child("kind")); }
bool InlayHint::has_kind() const { return n_.has_child("kind"); }
void InlayHint::set_kind(InlayHintKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
List<TextEdit> InlayHint::textEdits() { return n_.find_child("textEdits"); }
bool InlayHint::has_textEdits() const { return n_.has_child("textEdits"); }
List<TextEdit> InlayHint::add_textEdits() { auto w = List<TextEdit>(n_.append_child() << ryml::key("textEdits")); w.init(); return w; }
bool InlayHint::Union4::holds_string() const { return n_.has_child("TODO"); }
std::string_view InlayHint::Union4::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void InlayHint::Union4::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool InlayHint::Union4::holds_MarkupContent() const { return n_.has_child("TODO"); }
MarkupContent InlayHint::Union4::as_MarkupContent() { return {n_}; }
InlayHint::Union4 InlayHint::tooltip() { return n_.find_child("tooltip"); }
bool InlayHint::has_tooltip() const { return n_.has_child("tooltip"); }
InlayHint::Union4 InlayHint::add_tooltip() { auto w = InlayHint::Union4(n_.append_child() << ryml::key("tooltip")); w.init(); return w; }
bool InlayHint::paddingLeft() { return detail::NodeAsPrimitive<bool>(n_.find_child("paddingLeft")); }
bool InlayHint::has_paddingLeft() const { return n_.has_child("paddingLeft"); }
void InlayHint::set_paddingLeft(bool val) { auto c = n_.find_child("paddingLeft"); if (c.invalid()) { c = n_.append_child() << ryml::key("paddingLeft"); } detail::SetPrimitive(c, val); }
bool InlayHint::paddingRight() { return detail::NodeAsPrimitive<bool>(n_.find_child("paddingRight")); }
bool InlayHint::has_paddingRight() const { return n_.has_child("paddingRight"); }
void InlayHint::set_paddingRight(bool val) { auto c = n_.find_child("paddingRight"); if (c.invalid()) { c = n_.append_child() << ryml::key("paddingRight"); } detail::SetPrimitive(c, val); }
bool InlayHint::Union7::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject InlayHint::Union7::as_reference_Map0() {  }
bool InlayHint::Union7::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray InlayHint::Union7::as_reference_LSPAny() { return {n_}; }
bool InlayHint::Union7::holds_string() const { return n_.has_child("TODO"); }
std::string_view InlayHint::Union7::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void InlayHint::Union7::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool InlayHint::Union7::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t InlayHint::Union7::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void InlayHint::Union7::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool InlayHint::Union7::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t InlayHint::Union7::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void InlayHint::Union7::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool InlayHint::Union7::holds_decimal() const { return n_.has_child("TODO"); }
double InlayHint::Union7::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void InlayHint::Union7::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool InlayHint::Union7::holds_boolean() const { return n_.has_child("TODO"); }
bool InlayHint::Union7::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void InlayHint::Union7::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
InlayHint::Union7 InlayHint::data() { return n_.find_child("data"); }
bool InlayHint::has_data() const { return n_.has_child("data"); }
InlayHint::Union7 InlayHint::add_data() { auto w = InlayHint::Union7(n_.append_child() << ryml::key("data")); w.init(); return w; }
void InlayHint::init() { StructWrapper::init(); Position(n_.append_child() << ryml::key("position")).init(); InlayHint::Union1(n_.append_child() << ryml::key("label")).init(); }
bool InlayHintRegistrationOptions::resolveProvider() { return detail::NodeAsPrimitive<bool>(n_.find_child("resolveProvider")); }
bool InlayHintRegistrationOptions::has_resolveProvider() const { return n_.has_child("resolveProvider"); }
void InlayHintRegistrationOptions::set_resolveProvider(bool val) { auto c = n_.find_child("resolveProvider"); if (c.invalid()) { c = n_.append_child() << ryml::key("resolveProvider"); } detail::SetPrimitive(c, val); }
bool InlayHintRegistrationOptions::Union1::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector InlayHintRegistrationOptions::Union1::as_reference_DocumentFilter() { return {n_}; }
InlayHintRegistrationOptions::Union1 InlayHintRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
std::string_view InlayHintRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool InlayHintRegistrationOptions::has_id() const { return n_.has_child("id"); }
void InlayHintRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void InlayHintRegistrationOptions::init() { StructWrapper::init(); InlayHintRegistrationOptions::Union1(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier DocumentDiagnosticParams::textDocument() { return n_.find_child("textDocument"); }
std::string_view DocumentDiagnosticParams::identifier() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("identifier")); }
bool DocumentDiagnosticParams::has_identifier() const { return n_.has_child("identifier"); }
void DocumentDiagnosticParams::set_identifier(std::string_view val) { auto c = n_.find_child("identifier"); if (c.invalid()) { c = n_.append_child() << ryml::key("identifier"); } detail::SetPrimitive(c, val); }
std::string_view DocumentDiagnosticParams::previousResultId() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("previousResultId")); }
bool DocumentDiagnosticParams::has_previousResultId() const { return n_.has_child("previousResultId"); }
void DocumentDiagnosticParams::set_previousResultId(std::string_view val) { auto c = n_.find_child("previousResultId"); if (c.invalid()) { c = n_.append_child() << ryml::key("previousResultId"); } detail::SetPrimitive(c, val); }
bool DocumentDiagnosticParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DocumentDiagnosticParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DocumentDiagnosticParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DocumentDiagnosticParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view DocumentDiagnosticParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DocumentDiagnosticParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DocumentDiagnosticParams::Union3 DocumentDiagnosticParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool DocumentDiagnosticParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
DocumentDiagnosticParams::Union3 DocumentDiagnosticParams::add_workDoneToken() { auto w = DocumentDiagnosticParams::Union3(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool DocumentDiagnosticParams::Union4::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DocumentDiagnosticParams::Union4::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DocumentDiagnosticParams::Union4::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DocumentDiagnosticParams::Union4::holds_string() const { return n_.has_child("TODO"); }
std::string_view DocumentDiagnosticParams::Union4::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DocumentDiagnosticParams::Union4::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DocumentDiagnosticParams::Union4 DocumentDiagnosticParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool DocumentDiagnosticParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
DocumentDiagnosticParams::Union4 DocumentDiagnosticParams::add_partialResultToken() { auto w = DocumentDiagnosticParams::Union4(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void DocumentDiagnosticParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); }
Dict<std::string_view, std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>> DocumentDiagnosticReportPartialResult::relatedDocuments() { return n_.find_child("relatedDocuments"); }
void DocumentDiagnosticReportPartialResult::init() { StructWrapper::init(); Dict<std::string_view, std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>>(n_.append_child() << ryml::key("relatedDocuments")).init(); }
bool DiagnosticServerCancellationData::retriggerRequest() { return detail::NodeAsPrimitive<bool>(n_.find_child("retriggerRequest")); }
void DiagnosticServerCancellationData::set_retriggerRequest(bool val) { auto c = n_.find_child("retriggerRequest"); if (c.invalid()) { c = n_.append_child() << ryml::key("retriggerRequest"); } detail::SetPrimitive(c, val); }
void DiagnosticServerCancellationData::init() { StructWrapper::init();  }
bool DiagnosticRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector DiagnosticRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
DiagnosticRegistrationOptions::Union0 DiagnosticRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
std::string_view DiagnosticRegistrationOptions::identifier() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("identifier")); }
bool DiagnosticRegistrationOptions::has_identifier() const { return n_.has_child("identifier"); }
void DiagnosticRegistrationOptions::set_identifier(std::string_view val) { auto c = n_.find_child("identifier"); if (c.invalid()) { c = n_.append_child() << ryml::key("identifier"); } detail::SetPrimitive(c, val); }
bool DiagnosticRegistrationOptions::interFileDependencies() { return detail::NodeAsPrimitive<bool>(n_.find_child("interFileDependencies")); }
void DiagnosticRegistrationOptions::set_interFileDependencies(bool val) { auto c = n_.find_child("interFileDependencies"); if (c.invalid()) { c = n_.append_child() << ryml::key("interFileDependencies"); } detail::SetPrimitive(c, val); }
bool DiagnosticRegistrationOptions::workspaceDiagnostics() { return detail::NodeAsPrimitive<bool>(n_.find_child("workspaceDiagnostics")); }
void DiagnosticRegistrationOptions::set_workspaceDiagnostics(bool val) { auto c = n_.find_child("workspaceDiagnostics"); if (c.invalid()) { c = n_.append_child() << ryml::key("workspaceDiagnostics"); } detail::SetPrimitive(c, val); }
std::string_view DiagnosticRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool DiagnosticRegistrationOptions::has_id() const { return n_.has_child("id"); }
void DiagnosticRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void DiagnosticRegistrationOptions::init() { StructWrapper::init(); DiagnosticRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
std::string_view WorkspaceDiagnosticParams::identifier() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("identifier")); }
bool WorkspaceDiagnosticParams::has_identifier() const { return n_.has_child("identifier"); }
void WorkspaceDiagnosticParams::set_identifier(std::string_view val) { auto c = n_.find_child("identifier"); if (c.invalid()) { c = n_.append_child() << ryml::key("identifier"); } detail::SetPrimitive(c, val); }
List<PreviousResultId> WorkspaceDiagnosticParams::previousResultIds() { return n_.find_child("previousResultIds"); }
bool WorkspaceDiagnosticParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t WorkspaceDiagnosticParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void WorkspaceDiagnosticParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool WorkspaceDiagnosticParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view WorkspaceDiagnosticParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void WorkspaceDiagnosticParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
WorkspaceDiagnosticParams::Union2 WorkspaceDiagnosticParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool WorkspaceDiagnosticParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
WorkspaceDiagnosticParams::Union2 WorkspaceDiagnosticParams::add_workDoneToken() { auto w = WorkspaceDiagnosticParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool WorkspaceDiagnosticParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t WorkspaceDiagnosticParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void WorkspaceDiagnosticParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool WorkspaceDiagnosticParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view WorkspaceDiagnosticParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void WorkspaceDiagnosticParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
WorkspaceDiagnosticParams::Union3 WorkspaceDiagnosticParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool WorkspaceDiagnosticParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
WorkspaceDiagnosticParams::Union3 WorkspaceDiagnosticParams::add_partialResultToken() { auto w = WorkspaceDiagnosticParams::Union3(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void WorkspaceDiagnosticParams::init() { StructWrapper::init(); List<PreviousResultId>(n_.append_child() << ryml::key("previousResultIds")).init(); }
List<WorkspaceDocumentDiagnosticReport> WorkspaceDiagnosticReport::items() { return n_.find_child("items"); }
void WorkspaceDiagnosticReport::init() { StructWrapper::init(); List<WorkspaceDocumentDiagnosticReport>(n_.append_child() << ryml::key("items")).init(); }
List<WorkspaceDocumentDiagnosticReport> WorkspaceDiagnosticReportPartialResult::items() { return n_.find_child("items"); }
void WorkspaceDiagnosticReportPartialResult::init() { StructWrapper::init(); List<WorkspaceDocumentDiagnosticReport>(n_.append_child() << ryml::key("items")).init(); }
NotebookDocument DidOpenNotebookDocumentParams::notebookDocument() { return n_.find_child("notebookDocument"); }
List<TextDocumentItem> DidOpenNotebookDocumentParams::cellTextDocuments() { return n_.find_child("cellTextDocuments"); }
void DidOpenNotebookDocumentParams::init() { StructWrapper::init(); NotebookDocument(n_.append_child() << ryml::key("notebookDocument")).init(); List<TextDocumentItem>(n_.append_child() << ryml::key("cellTextDocuments")).init(); }
List<std::variant<NotebookDocumentFilterWithNotebook, NotebookDocumentFilterWithCells>> NotebookDocumentSyncRegistrationOptions::notebookSelector() { return n_.find_child("notebookSelector"); }
bool NotebookDocumentSyncRegistrationOptions::save() { return detail::NodeAsPrimitive<bool>(n_.find_child("save")); }
bool NotebookDocumentSyncRegistrationOptions::has_save() const { return n_.has_child("save"); }
void NotebookDocumentSyncRegistrationOptions::set_save(bool val) { auto c = n_.find_child("save"); if (c.invalid()) { c = n_.append_child() << ryml::key("save"); } detail::SetPrimitive(c, val); }
std::string_view NotebookDocumentSyncRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool NotebookDocumentSyncRegistrationOptions::has_id() const { return n_.has_child("id"); }
void NotebookDocumentSyncRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void NotebookDocumentSyncRegistrationOptions::init() { StructWrapper::init(); List<std::variant<NotebookDocumentFilterWithNotebook, NotebookDocumentFilterWithCells>>(n_.append_child() << ryml::key("notebookSelector")).init(); }
VersionedNotebookDocumentIdentifier DidChangeNotebookDocumentParams::notebookDocument() { return n_.find_child("notebookDocument"); }
NotebookDocumentChangeEvent DidChangeNotebookDocumentParams::change() { return n_.find_child("change"); }
void DidChangeNotebookDocumentParams::init() { StructWrapper::init(); VersionedNotebookDocumentIdentifier(n_.append_child() << ryml::key("notebookDocument")).init(); NotebookDocumentChangeEvent(n_.append_child() << ryml::key("change")).init(); }
NotebookDocumentIdentifier DidSaveNotebookDocumentParams::notebookDocument() { return n_.find_child("notebookDocument"); }
void DidSaveNotebookDocumentParams::init() { StructWrapper::init(); NotebookDocumentIdentifier(n_.append_child() << ryml::key("notebookDocument")).init(); }
NotebookDocumentIdentifier DidCloseNotebookDocumentParams::notebookDocument() { return n_.find_child("notebookDocument"); }
List<TextDocumentIdentifier> DidCloseNotebookDocumentParams::cellTextDocuments() { return n_.find_child("cellTextDocuments"); }
void DidCloseNotebookDocumentParams::init() { StructWrapper::init(); NotebookDocumentIdentifier(n_.append_child() << ryml::key("notebookDocument")).init(); List<TextDocumentIdentifier>(n_.append_child() << ryml::key("cellTextDocuments")).init(); }
InlineCompletionContext InlineCompletionParams::context() { return n_.find_child("context"); }
TextDocumentIdentifier InlineCompletionParams::textDocument() { return n_.find_child("textDocument"); }
Position InlineCompletionParams::position() { return n_.find_child("position"); }
bool InlineCompletionParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t InlineCompletionParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void InlineCompletionParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool InlineCompletionParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view InlineCompletionParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void InlineCompletionParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
InlineCompletionParams::Union3 InlineCompletionParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool InlineCompletionParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
InlineCompletionParams::Union3 InlineCompletionParams::add_workDoneToken() { auto w = InlineCompletionParams::Union3(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void InlineCompletionParams::init() { StructWrapper::init(); InlineCompletionContext(n_.append_child() << ryml::key("context")).init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
List<InlineCompletionItem> InlineCompletionList::items() { return n_.find_child("items"); }
void InlineCompletionList::init() { StructWrapper::init(); List<InlineCompletionItem>(n_.append_child() << ryml::key("items")).init(); }
bool InlineCompletionItem::Union0::holds_string() const { return n_.has_child("TODO"); }
std::string_view InlineCompletionItem::Union0::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void InlineCompletionItem::Union0::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool InlineCompletionItem::Union0::holds_StringValue() const { return n_.has_child("TODO"); }
StringValue InlineCompletionItem::Union0::as_StringValue() { return {n_}; }
InlineCompletionItem::Union0 InlineCompletionItem::insertText() { return n_.find_child("insertText"); }
std::string_view InlineCompletionItem::filterText() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("filterText")); }
bool InlineCompletionItem::has_filterText() const { return n_.has_child("filterText"); }
void InlineCompletionItem::set_filterText(std::string_view val) { auto c = n_.find_child("filterText"); if (c.invalid()) { c = n_.append_child() << ryml::key("filterText"); } detail::SetPrimitive(c, val); }
Range InlineCompletionItem::range() { return n_.find_child("range"); }
bool InlineCompletionItem::has_range() const { return n_.has_child("range"); }
Range InlineCompletionItem::add_range() { auto w = Range(n_.append_child() << ryml::key("range")); w.init(); return w; }
Command InlineCompletionItem::command() { return n_.find_child("command"); }
bool InlineCompletionItem::has_command() const { return n_.has_child("command"); }
Command InlineCompletionItem::add_command() { auto w = Command(n_.append_child() << ryml::key("command")); w.init(); return w; }
void InlineCompletionItem::init() { StructWrapper::init(); InlineCompletionItem::Union0(n_.append_child() << ryml::key("insertText")).init(); }
bool InlineCompletionRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector InlineCompletionRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
InlineCompletionRegistrationOptions::Union0 InlineCompletionRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
std::string_view InlineCompletionRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool InlineCompletionRegistrationOptions::has_id() const { return n_.has_child("id"); }
void InlineCompletionRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void InlineCompletionRegistrationOptions::init() { StructWrapper::init(); InlineCompletionRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
std::string_view TextDocumentContentParams::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void TextDocumentContentParams::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
void TextDocumentContentParams::init() { StructWrapper::init();  }
std::string_view TextDocumentContentResult::text() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("text")); }
void TextDocumentContentResult::set_text(std::string_view val) { auto c = n_.find_child("text"); if (c.invalid()) { c = n_.append_child() << ryml::key("text"); } detail::SetPrimitive(c, val); }
void TextDocumentContentResult::init() { StructWrapper::init();  }
List<std::string_view> TextDocumentContentRegistrationOptions::schemes() { return n_.find_child("schemes"); }
std::string_view TextDocumentContentRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool TextDocumentContentRegistrationOptions::has_id() const { return n_.has_child("id"); }
void TextDocumentContentRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void TextDocumentContentRegistrationOptions::init() { StructWrapper::init(); List<std::string_view>(n_.append_child() << ryml::key("schemes")).init(); }
std::string_view TextDocumentContentRefreshParams::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void TextDocumentContentRefreshParams::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
void TextDocumentContentRefreshParams::init() { StructWrapper::init();  }
List<Registration> RegistrationParams::registrations() { return n_.find_child("registrations"); }
void RegistrationParams::init() { StructWrapper::init(); List<Registration>(n_.append_child() << ryml::key("registrations")).init(); }
List<Unregistration> UnregistrationParams::unregisterations() { return n_.find_child("unregisterations"); }
void UnregistrationParams::init() { StructWrapper::init(); List<Unregistration>(n_.append_child() << ryml::key("unregisterations")).init(); }
bool InitializeParams::Union0::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t InitializeParams::Union0::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void InitializeParams::Union0::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
InitializeParams::Union0 InitializeParams::processId() { return n_.find_child("processId"); }
ClientInfo InitializeParams::clientInfo() { return n_.find_child("clientInfo"); }
bool InitializeParams::has_clientInfo() const { return n_.has_child("clientInfo"); }
ClientInfo InitializeParams::add_clientInfo() { auto w = ClientInfo(n_.append_child() << ryml::key("clientInfo")); w.init(); return w; }
std::string_view InitializeParams::locale() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("locale")); }
bool InitializeParams::has_locale() const { return n_.has_child("locale"); }
void InitializeParams::set_locale(std::string_view val) { auto c = n_.find_child("locale"); if (c.invalid()) { c = n_.append_child() << ryml::key("locale"); } detail::SetPrimitive(c, val); }
bool InitializeParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view InitializeParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void InitializeParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
InitializeParams::Union3 InitializeParams::rootPath() { return n_.find_child("rootPath"); }
bool InitializeParams::has_rootPath() const { return n_.has_child("rootPath"); }
InitializeParams::Union3 InitializeParams::add_rootPath() { auto w = InitializeParams::Union3(n_.append_child() << ryml::key("rootPath")); w.init(); return w; }
bool InitializeParams::Union4::holds_DocumentUri() const { return n_.has_child("TODO"); }
std::string_view InitializeParams::Union4::as_DocumentUri() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void InitializeParams::Union4::set_DocumentUri(std::string_view val) { detail::SetPrimitive(n_, val); }
InitializeParams::Union4 InitializeParams::rootUri() { return n_.find_child("rootUri"); }
ClientCapabilities InitializeParams::capabilities() { return n_.find_child("capabilities"); }
bool InitializeParams::Union6::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject InitializeParams::Union6::as_reference_Map0() {  }
bool InitializeParams::Union6::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray InitializeParams::Union6::as_reference_LSPAny() { return {n_}; }
bool InitializeParams::Union6::holds_string() const { return n_.has_child("TODO"); }
std::string_view InitializeParams::Union6::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void InitializeParams::Union6::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool InitializeParams::Union6::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t InitializeParams::Union6::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void InitializeParams::Union6::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool InitializeParams::Union6::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t InitializeParams::Union6::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void InitializeParams::Union6::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool InitializeParams::Union6::holds_decimal() const { return n_.has_child("TODO"); }
double InitializeParams::Union6::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void InitializeParams::Union6::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool InitializeParams::Union6::holds_boolean() const { return n_.has_child("TODO"); }
bool InitializeParams::Union6::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void InitializeParams::Union6::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
InitializeParams::Union6 InitializeParams::initializationOptions() { return n_.find_child("initializationOptions"); }
bool InitializeParams::has_initializationOptions() const { return n_.has_child("initializationOptions"); }
InitializeParams::Union6 InitializeParams::add_initializationOptions() { auto w = InitializeParams::Union6(n_.append_child() << ryml::key("initializationOptions")); w.init(); return w; }
TraceValue InitializeParams::trace() { return detail::DeserializeEnum<TraceValue>(n_.find_child("trace")); }
bool InitializeParams::has_trace() const { return n_.has_child("trace"); }
void InitializeParams::set_trace(TraceValue val) { auto c = n_.find_child("trace"); if (c.invalid()) { c = n_.append_child() << ryml::key("trace"); } detail::SerializeEnum(val, c); }
bool InitializeParams::Union8::holds_array_WorkspaceFolder() const { return n_.has_child("TODO"); }
List<WorkspaceFolder> InitializeParams::Union8::as_array_WorkspaceFolder() { return {n_}; }
InitializeParams::Union8 InitializeParams::workspaceFolders() { return n_.find_child("workspaceFolders"); }
bool InitializeParams::has_workspaceFolders() const { return n_.has_child("workspaceFolders"); }
InitializeParams::Union8 InitializeParams::add_workspaceFolders() { auto w = InitializeParams::Union8(n_.append_child() << ryml::key("workspaceFolders")); w.init(); return w; }
void InitializeParams::init() { StructWrapper::init(); InitializeParams::Union0(n_.append_child() << ryml::key("processId")).init(); InitializeParams::Union4(n_.append_child() << ryml::key("rootUri")).init(); ClientCapabilities(n_.append_child() << ryml::key("capabilities")).init(); }
ServerCapabilities InitializeResult::capabilities() { return n_.find_child("capabilities"); }
ServerInfo InitializeResult::serverInfo() { return n_.find_child("serverInfo"); }
bool InitializeResult::has_serverInfo() const { return n_.has_child("serverInfo"); }
ServerInfo InitializeResult::add_serverInfo() { auto w = ServerInfo(n_.append_child() << ryml::key("serverInfo")); w.init(); return w; }
void InitializeResult::init() { StructWrapper::init(); ServerCapabilities(n_.append_child() << ryml::key("capabilities")).init(); }
bool InitializeError::retry() { return detail::NodeAsPrimitive<bool>(n_.find_child("retry")); }
void InitializeError::set_retry(bool val) { auto c = n_.find_child("retry"); if (c.invalid()) { c = n_.append_child() << ryml::key("retry"); } detail::SetPrimitive(c, val); }
void InitializeError::init() { StructWrapper::init();  }
void InitializedParams::init() { StructWrapper::init();  }
bool DidChangeConfigurationParams::Union0::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject DidChangeConfigurationParams::Union0::as_reference_Map0() {  }
bool DidChangeConfigurationParams::Union0::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray DidChangeConfigurationParams::Union0::as_reference_LSPAny() { return {n_}; }
bool DidChangeConfigurationParams::Union0::holds_string() const { return n_.has_child("TODO"); }
std::string_view DidChangeConfigurationParams::Union0::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DidChangeConfigurationParams::Union0::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool DidChangeConfigurationParams::Union0::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DidChangeConfigurationParams::Union0::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DidChangeConfigurationParams::Union0::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DidChangeConfigurationParams::Union0::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t DidChangeConfigurationParams::Union0::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void DidChangeConfigurationParams::Union0::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool DidChangeConfigurationParams::Union0::holds_decimal() const { return n_.has_child("TODO"); }
double DidChangeConfigurationParams::Union0::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void DidChangeConfigurationParams::Union0::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool DidChangeConfigurationParams::Union0::holds_boolean() const { return n_.has_child("TODO"); }
bool DidChangeConfigurationParams::Union0::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void DidChangeConfigurationParams::Union0::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
DidChangeConfigurationParams::Union0 DidChangeConfigurationParams::settings() { return n_.find_child("settings"); }
void DidChangeConfigurationParams::init() { StructWrapper::init(); DidChangeConfigurationParams::Union0(n_.append_child() << ryml::key("settings")).init(); }
bool DidChangeConfigurationRegistrationOptions::Union0::holds_string() const { return n_.has_child("TODO"); }
std::string_view DidChangeConfigurationRegistrationOptions::Union0::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DidChangeConfigurationRegistrationOptions::Union0::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool DidChangeConfigurationRegistrationOptions::Union0::holds_array_string() const { return n_.has_child("TODO"); }
List<std::string_view> DidChangeConfigurationRegistrationOptions::Union0::as_array_string() { return {n_}; }
DidChangeConfigurationRegistrationOptions::Union0 DidChangeConfigurationRegistrationOptions::section() { return n_.find_child("section"); }
bool DidChangeConfigurationRegistrationOptions::has_section() const { return n_.has_child("section"); }
DidChangeConfigurationRegistrationOptions::Union0 DidChangeConfigurationRegistrationOptions::add_section() { auto w = DidChangeConfigurationRegistrationOptions::Union0(n_.append_child() << ryml::key("section")); w.init(); return w; }
void DidChangeConfigurationRegistrationOptions::init() { StructWrapper::init();  }
MessageType ShowMessageParams::type() { return detail::DeserializeEnum<MessageType>(n_.find_child("type")); }
void ShowMessageParams::set_type(MessageType val) { auto c = n_.find_child("type"); if (c.invalid()) { c = n_.append_child() << ryml::key("type"); } detail::SerializeEnum(val, c); }
std::string_view ShowMessageParams::message() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("message")); }
void ShowMessageParams::set_message(std::string_view val) { auto c = n_.find_child("message"); if (c.invalid()) { c = n_.append_child() << ryml::key("message"); } detail::SetPrimitive(c, val); }
void ShowMessageParams::init() { StructWrapper::init();  }
MessageType ShowMessageRequestParams::type() { return detail::DeserializeEnum<MessageType>(n_.find_child("type")); }
void ShowMessageRequestParams::set_type(MessageType val) { auto c = n_.find_child("type"); if (c.invalid()) { c = n_.append_child() << ryml::key("type"); } detail::SerializeEnum(val, c); }
std::string_view ShowMessageRequestParams::message() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("message")); }
void ShowMessageRequestParams::set_message(std::string_view val) { auto c = n_.find_child("message"); if (c.invalid()) { c = n_.append_child() << ryml::key("message"); } detail::SetPrimitive(c, val); }
List<MessageActionItem> ShowMessageRequestParams::actions() { return n_.find_child("actions"); }
bool ShowMessageRequestParams::has_actions() const { return n_.has_child("actions"); }
List<MessageActionItem> ShowMessageRequestParams::add_actions() { auto w = List<MessageActionItem>(n_.append_child() << ryml::key("actions")); w.init(); return w; }
void ShowMessageRequestParams::init() { StructWrapper::init();  }
std::string_view MessageActionItem::title() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("title")); }
void MessageActionItem::set_title(std::string_view val) { auto c = n_.find_child("title"); if (c.invalid()) { c = n_.append_child() << ryml::key("title"); } detail::SetPrimitive(c, val); }
void MessageActionItem::init() { StructWrapper::init();  }
MessageType LogMessageParams::type() { return detail::DeserializeEnum<MessageType>(n_.find_child("type")); }
void LogMessageParams::set_type(MessageType val) { auto c = n_.find_child("type"); if (c.invalid()) { c = n_.append_child() << ryml::key("type"); } detail::SerializeEnum(val, c); }
std::string_view LogMessageParams::message() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("message")); }
void LogMessageParams::set_message(std::string_view val) { auto c = n_.find_child("message"); if (c.invalid()) { c = n_.append_child() << ryml::key("message"); } detail::SetPrimitive(c, val); }
void LogMessageParams::init() { StructWrapper::init();  }
TextDocumentItem DidOpenTextDocumentParams::textDocument() { return n_.find_child("textDocument"); }
void DidOpenTextDocumentParams::init() { StructWrapper::init(); TextDocumentItem(n_.append_child() << ryml::key("textDocument")).init(); }
VersionedTextDocumentIdentifier DidChangeTextDocumentParams::textDocument() { return n_.find_child("textDocument"); }
List<TextDocumentContentChangeEvent> DidChangeTextDocumentParams::contentChanges() { return n_.find_child("contentChanges"); }
void DidChangeTextDocumentParams::init() { StructWrapper::init(); VersionedTextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); List<TextDocumentContentChangeEvent>(n_.append_child() << ryml::key("contentChanges")).init(); }
TextDocumentSyncKind TextDocumentChangeRegistrationOptions::syncKind() { return detail::DeserializeEnum<TextDocumentSyncKind>(n_.find_child("syncKind")); }
void TextDocumentChangeRegistrationOptions::set_syncKind(TextDocumentSyncKind val) { auto c = n_.find_child("syncKind"); if (c.invalid()) { c = n_.append_child() << ryml::key("syncKind"); } detail::SerializeEnum(val, c); }
bool TextDocumentChangeRegistrationOptions::Union1::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector TextDocumentChangeRegistrationOptions::Union1::as_reference_DocumentFilter() { return {n_}; }
TextDocumentChangeRegistrationOptions::Union1 TextDocumentChangeRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
void TextDocumentChangeRegistrationOptions::init() { StructWrapper::init(); TextDocumentChangeRegistrationOptions::Union1(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier DidCloseTextDocumentParams::textDocument() { return n_.find_child("textDocument"); }
void DidCloseTextDocumentParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); }
TextDocumentIdentifier DidSaveTextDocumentParams::textDocument() { return n_.find_child("textDocument"); }
std::string_view DidSaveTextDocumentParams::text() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("text")); }
bool DidSaveTextDocumentParams::has_text() const { return n_.has_child("text"); }
void DidSaveTextDocumentParams::set_text(std::string_view val) { auto c = n_.find_child("text"); if (c.invalid()) { c = n_.append_child() << ryml::key("text"); } detail::SetPrimitive(c, val); }
void DidSaveTextDocumentParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); }
bool TextDocumentSaveRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector TextDocumentSaveRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
TextDocumentSaveRegistrationOptions::Union0 TextDocumentSaveRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
bool TextDocumentSaveRegistrationOptions::includeText() { return detail::NodeAsPrimitive<bool>(n_.find_child("includeText")); }
bool TextDocumentSaveRegistrationOptions::has_includeText() const { return n_.has_child("includeText"); }
void TextDocumentSaveRegistrationOptions::set_includeText(bool val) { auto c = n_.find_child("includeText"); if (c.invalid()) { c = n_.append_child() << ryml::key("includeText"); } detail::SetPrimitive(c, val); }
void TextDocumentSaveRegistrationOptions::init() { StructWrapper::init(); TextDocumentSaveRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier WillSaveTextDocumentParams::textDocument() { return n_.find_child("textDocument"); }
TextDocumentSaveReason WillSaveTextDocumentParams::reason() { return detail::DeserializeEnum<TextDocumentSaveReason>(n_.find_child("reason")); }
void WillSaveTextDocumentParams::set_reason(TextDocumentSaveReason val) { auto c = n_.find_child("reason"); if (c.invalid()) { c = n_.append_child() << ryml::key("reason"); } detail::SerializeEnum(val, c); }
void WillSaveTextDocumentParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); }
Range TextEdit::range() { return n_.find_child("range"); }
std::string_view TextEdit::newText() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("newText")); }
void TextEdit::set_newText(std::string_view val) { auto c = n_.find_child("newText"); if (c.invalid()) { c = n_.append_child() << ryml::key("newText"); } detail::SetPrimitive(c, val); }
void TextEdit::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); }
List<FileEvent> DidChangeWatchedFilesParams::changes() { return n_.find_child("changes"); }
void DidChangeWatchedFilesParams::init() { StructWrapper::init(); List<FileEvent>(n_.append_child() << ryml::key("changes")).init(); }
List<FileSystemWatcher> DidChangeWatchedFilesRegistrationOptions::watchers() { return n_.find_child("watchers"); }
void DidChangeWatchedFilesRegistrationOptions::init() { StructWrapper::init(); List<FileSystemWatcher>(n_.append_child() << ryml::key("watchers")).init(); }
std::string_view PublishDiagnosticsParams::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void PublishDiagnosticsParams::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
std::int32_t PublishDiagnosticsParams::version() { return detail::NodeAsPrimitive<std::int32_t>(n_.find_child("version")); }
bool PublishDiagnosticsParams::has_version() const { return n_.has_child("version"); }
void PublishDiagnosticsParams::set_version(std::int32_t val) { auto c = n_.find_child("version"); if (c.invalid()) { c = n_.append_child() << ryml::key("version"); } detail::SetPrimitive(c, val); }
List<Diagnostic> PublishDiagnosticsParams::diagnostics() { return n_.find_child("diagnostics"); }
void PublishDiagnosticsParams::init() { StructWrapper::init(); List<Diagnostic>(n_.append_child() << ryml::key("diagnostics")).init(); }
CompletionContext CompletionParams::context() { return n_.find_child("context"); }
bool CompletionParams::has_context() const { return n_.has_child("context"); }
CompletionContext CompletionParams::add_context() { auto w = CompletionContext(n_.append_child() << ryml::key("context")); w.init(); return w; }
TextDocumentIdentifier CompletionParams::textDocument() { return n_.find_child("textDocument"); }
Position CompletionParams::position() { return n_.find_child("position"); }
bool CompletionParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CompletionParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CompletionParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CompletionParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view CompletionParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CompletionParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
CompletionParams::Union3 CompletionParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool CompletionParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
CompletionParams::Union3 CompletionParams::add_workDoneToken() { auto w = CompletionParams::Union3(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool CompletionParams::Union4::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CompletionParams::Union4::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CompletionParams::Union4::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CompletionParams::Union4::holds_string() const { return n_.has_child("TODO"); }
std::string_view CompletionParams::Union4::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CompletionParams::Union4::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
CompletionParams::Union4 CompletionParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool CompletionParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
CompletionParams::Union4 CompletionParams::add_partialResultToken() { auto w = CompletionParams::Union4(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void CompletionParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
std::string_view CompletionItem::label() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("label")); }
void CompletionItem::set_label(std::string_view val) { auto c = n_.find_child("label"); if (c.invalid()) { c = n_.append_child() << ryml::key("label"); } detail::SetPrimitive(c, val); }
CompletionItemLabelDetails CompletionItem::labelDetails() { return n_.find_child("labelDetails"); }
bool CompletionItem::has_labelDetails() const { return n_.has_child("labelDetails"); }
CompletionItemLabelDetails CompletionItem::add_labelDetails() { auto w = CompletionItemLabelDetails(n_.append_child() << ryml::key("labelDetails")); w.init(); return w; }
CompletionItemKind CompletionItem::kind() { return detail::DeserializeEnum<CompletionItemKind>(n_.find_child("kind")); }
bool CompletionItem::has_kind() const { return n_.has_child("kind"); }
void CompletionItem::set_kind(CompletionItemKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
List<CompletionItemTag> CompletionItem::tags() { return n_.find_child("tags"); }
bool CompletionItem::has_tags() const { return n_.has_child("tags"); }
List<CompletionItemTag> CompletionItem::add_tags() { auto w = List<CompletionItemTag>(n_.append_child() << ryml::key("tags")); w.init(); return w; }
std::string_view CompletionItem::detail() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("detail")); }
bool CompletionItem::has_detail() const { return n_.has_child("detail"); }
void CompletionItem::set_detail(std::string_view val) { auto c = n_.find_child("detail"); if (c.invalid()) { c = n_.append_child() << ryml::key("detail"); } detail::SetPrimitive(c, val); }
bool CompletionItem::Union5::holds_string() const { return n_.has_child("TODO"); }
std::string_view CompletionItem::Union5::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CompletionItem::Union5::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool CompletionItem::Union5::holds_MarkupContent() const { return n_.has_child("TODO"); }
MarkupContent CompletionItem::Union5::as_MarkupContent() { return {n_}; }
CompletionItem::Union5 CompletionItem::documentation() { return n_.find_child("documentation"); }
bool CompletionItem::has_documentation() const { return n_.has_child("documentation"); }
CompletionItem::Union5 CompletionItem::add_documentation() { auto w = CompletionItem::Union5(n_.append_child() << ryml::key("documentation")); w.init(); return w; }
bool CompletionItem::deprecated() { return detail::NodeAsPrimitive<bool>(n_.find_child("deprecated")); }
bool CompletionItem::has_deprecated() const { return n_.has_child("deprecated"); }
void CompletionItem::set_deprecated(bool val) { auto c = n_.find_child("deprecated"); if (c.invalid()) { c = n_.append_child() << ryml::key("deprecated"); } detail::SetPrimitive(c, val); }
bool CompletionItem::preselect() { return detail::NodeAsPrimitive<bool>(n_.find_child("preselect")); }
bool CompletionItem::has_preselect() const { return n_.has_child("preselect"); }
void CompletionItem::set_preselect(bool val) { auto c = n_.find_child("preselect"); if (c.invalid()) { c = n_.append_child() << ryml::key("preselect"); } detail::SetPrimitive(c, val); }
std::string_view CompletionItem::sortText() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("sortText")); }
bool CompletionItem::has_sortText() const { return n_.has_child("sortText"); }
void CompletionItem::set_sortText(std::string_view val) { auto c = n_.find_child("sortText"); if (c.invalid()) { c = n_.append_child() << ryml::key("sortText"); } detail::SetPrimitive(c, val); }
std::string_view CompletionItem::filterText() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("filterText")); }
bool CompletionItem::has_filterText() const { return n_.has_child("filterText"); }
void CompletionItem::set_filterText(std::string_view val) { auto c = n_.find_child("filterText"); if (c.invalid()) { c = n_.append_child() << ryml::key("filterText"); } detail::SetPrimitive(c, val); }
std::string_view CompletionItem::insertText() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("insertText")); }
bool CompletionItem::has_insertText() const { return n_.has_child("insertText"); }
void CompletionItem::set_insertText(std::string_view val) { auto c = n_.find_child("insertText"); if (c.invalid()) { c = n_.append_child() << ryml::key("insertText"); } detail::SetPrimitive(c, val); }
InsertTextFormat CompletionItem::insertTextFormat() { return detail::DeserializeEnum<InsertTextFormat>(n_.find_child("insertTextFormat")); }
bool CompletionItem::has_insertTextFormat() const { return n_.has_child("insertTextFormat"); }
void CompletionItem::set_insertTextFormat(InsertTextFormat val) { auto c = n_.find_child("insertTextFormat"); if (c.invalid()) { c = n_.append_child() << ryml::key("insertTextFormat"); } detail::SerializeEnum(val, c); }
InsertTextMode CompletionItem::insertTextMode() { return detail::DeserializeEnum<InsertTextMode>(n_.find_child("insertTextMode")); }
bool CompletionItem::has_insertTextMode() const { return n_.has_child("insertTextMode"); }
void CompletionItem::set_insertTextMode(InsertTextMode val) { auto c = n_.find_child("insertTextMode"); if (c.invalid()) { c = n_.append_child() << ryml::key("insertTextMode"); } detail::SerializeEnum(val, c); }
bool CompletionItem::Union13::holds_TextEdit() const { return n_.has_child("TODO"); }
TextEdit CompletionItem::Union13::as_TextEdit() { return {n_}; }
bool CompletionItem::Union13::holds_InsertReplaceEdit() const { return n_.has_child("TODO"); }
InsertReplaceEdit CompletionItem::Union13::as_InsertReplaceEdit() { return {n_}; }
CompletionItem::Union13 CompletionItem::textEdit() { return n_.find_child("textEdit"); }
bool CompletionItem::has_textEdit() const { return n_.has_child("textEdit"); }
CompletionItem::Union13 CompletionItem::add_textEdit() { auto w = CompletionItem::Union13(n_.append_child() << ryml::key("textEdit")); w.init(); return w; }
std::string_view CompletionItem::textEditText() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("textEditText")); }
bool CompletionItem::has_textEditText() const { return n_.has_child("textEditText"); }
void CompletionItem::set_textEditText(std::string_view val) { auto c = n_.find_child("textEditText"); if (c.invalid()) { c = n_.append_child() << ryml::key("textEditText"); } detail::SetPrimitive(c, val); }
List<TextEdit> CompletionItem::additionalTextEdits() { return n_.find_child("additionalTextEdits"); }
bool CompletionItem::has_additionalTextEdits() const { return n_.has_child("additionalTextEdits"); }
List<TextEdit> CompletionItem::add_additionalTextEdits() { auto w = List<TextEdit>(n_.append_child() << ryml::key("additionalTextEdits")); w.init(); return w; }
List<std::string_view> CompletionItem::commitCharacters() { return n_.find_child("commitCharacters"); }
bool CompletionItem::has_commitCharacters() const { return n_.has_child("commitCharacters"); }
List<std::string_view> CompletionItem::add_commitCharacters() { auto w = List<std::string_view>(n_.append_child() << ryml::key("commitCharacters")); w.init(); return w; }
Command CompletionItem::command() { return n_.find_child("command"); }
bool CompletionItem::has_command() const { return n_.has_child("command"); }
Command CompletionItem::add_command() { auto w = Command(n_.append_child() << ryml::key("command")); w.init(); return w; }
bool CompletionItem::Union18::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject CompletionItem::Union18::as_reference_Map0() {  }
bool CompletionItem::Union18::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray CompletionItem::Union18::as_reference_LSPAny() { return {n_}; }
bool CompletionItem::Union18::holds_string() const { return n_.has_child("TODO"); }
std::string_view CompletionItem::Union18::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CompletionItem::Union18::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool CompletionItem::Union18::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CompletionItem::Union18::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CompletionItem::Union18::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CompletionItem::Union18::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t CompletionItem::Union18::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void CompletionItem::Union18::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool CompletionItem::Union18::holds_decimal() const { return n_.has_child("TODO"); }
double CompletionItem::Union18::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void CompletionItem::Union18::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool CompletionItem::Union18::holds_boolean() const { return n_.has_child("TODO"); }
bool CompletionItem::Union18::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void CompletionItem::Union18::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
CompletionItem::Union18 CompletionItem::data() { return n_.find_child("data"); }
bool CompletionItem::has_data() const { return n_.has_child("data"); }
CompletionItem::Union18 CompletionItem::add_data() { auto w = CompletionItem::Union18(n_.append_child() << ryml::key("data")); w.init(); return w; }
void CompletionItem::init() { StructWrapper::init();  }
bool CompletionList::isIncomplete() { return detail::NodeAsPrimitive<bool>(n_.find_child("isIncomplete")); }
void CompletionList::set_isIncomplete(bool val) { auto c = n_.find_child("isIncomplete"); if (c.invalid()) { c = n_.append_child() << ryml::key("isIncomplete"); } detail::SetPrimitive(c, val); }
CompletionItemDefaults CompletionList::itemDefaults() { return n_.find_child("itemDefaults"); }
bool CompletionList::has_itemDefaults() const { return n_.has_child("itemDefaults"); }
CompletionItemDefaults CompletionList::add_itemDefaults() { auto w = CompletionItemDefaults(n_.append_child() << ryml::key("itemDefaults")); w.init(); return w; }
CompletionItemApplyKinds CompletionList::applyKind() { return n_.find_child("applyKind"); }
bool CompletionList::has_applyKind() const { return n_.has_child("applyKind"); }
CompletionItemApplyKinds CompletionList::add_applyKind() { auto w = CompletionItemApplyKinds(n_.append_child() << ryml::key("applyKind")); w.init(); return w; }
List<CompletionItem> CompletionList::items() { return n_.find_child("items"); }
void CompletionList::init() { StructWrapper::init(); List<CompletionItem>(n_.append_child() << ryml::key("items")).init(); }
bool CompletionRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector CompletionRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
CompletionRegistrationOptions::Union0 CompletionRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
List<std::string_view> CompletionRegistrationOptions::triggerCharacters() { return n_.find_child("triggerCharacters"); }
bool CompletionRegistrationOptions::has_triggerCharacters() const { return n_.has_child("triggerCharacters"); }
List<std::string_view> CompletionRegistrationOptions::add_triggerCharacters() { auto w = List<std::string_view>(n_.append_child() << ryml::key("triggerCharacters")); w.init(); return w; }
List<std::string_view> CompletionRegistrationOptions::allCommitCharacters() { return n_.find_child("allCommitCharacters"); }
bool CompletionRegistrationOptions::has_allCommitCharacters() const { return n_.has_child("allCommitCharacters"); }
List<std::string_view> CompletionRegistrationOptions::add_allCommitCharacters() { auto w = List<std::string_view>(n_.append_child() << ryml::key("allCommitCharacters")); w.init(); return w; }
bool CompletionRegistrationOptions::resolveProvider() { return detail::NodeAsPrimitive<bool>(n_.find_child("resolveProvider")); }
bool CompletionRegistrationOptions::has_resolveProvider() const { return n_.has_child("resolveProvider"); }
void CompletionRegistrationOptions::set_resolveProvider(bool val) { auto c = n_.find_child("resolveProvider"); if (c.invalid()) { c = n_.append_child() << ryml::key("resolveProvider"); } detail::SetPrimitive(c, val); }
ServerCompletionItemOptions CompletionRegistrationOptions::completionItem() { return n_.find_child("completionItem"); }
bool CompletionRegistrationOptions::has_completionItem() const { return n_.has_child("completionItem"); }
ServerCompletionItemOptions CompletionRegistrationOptions::add_completionItem() { auto w = ServerCompletionItemOptions(n_.append_child() << ryml::key("completionItem")); w.init(); return w; }
void CompletionRegistrationOptions::init() { StructWrapper::init(); CompletionRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier HoverParams::textDocument() { return n_.find_child("textDocument"); }
Position HoverParams::position() { return n_.find_child("position"); }
bool HoverParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t HoverParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void HoverParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool HoverParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view HoverParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void HoverParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
HoverParams::Union2 HoverParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool HoverParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
HoverParams::Union2 HoverParams::add_workDoneToken() { auto w = HoverParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void HoverParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
bool Hover::Union0::holds_MarkupContent() const { return n_.has_child("TODO"); }
MarkupContent Hover::Union0::as_MarkupContent() { return {n_}; }
bool Hover::Union0::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view Hover::Union0::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void Hover::Union0::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool Hover::Union0::Union1::holds_MarkedStringWithLanguage() const { return n_.has_child("TODO"); }
MarkedStringWithLanguage Hover::Union0::Union1::as_MarkedStringWithLanguage() { return {n_}; }
bool Hover::Union0::holds_reference_Union1() const { return n_.has_child("TODO"); }
MarkedString Hover::Union0::as_reference_Union1() {  }
bool Hover::Union0::holds_array_MarkedString() const { return n_.has_child("TODO"); }
List<MarkedString> Hover::Union0::as_array_MarkedString() { return {n_}; }
Hover::Union0 Hover::contents() { return n_.find_child("contents"); }
Range Hover::range() { return n_.find_child("range"); }
bool Hover::has_range() const { return n_.has_child("range"); }
Range Hover::add_range() { auto w = Range(n_.append_child() << ryml::key("range")); w.init(); return w; }
void Hover::init() { StructWrapper::init(); Hover::Union0(n_.append_child() << ryml::key("contents")).init(); }
bool HoverRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector HoverRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
HoverRegistrationOptions::Union0 HoverRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
void HoverRegistrationOptions::init() { StructWrapper::init(); HoverRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
SignatureHelpContext SignatureHelpParams::context() { return n_.find_child("context"); }
bool SignatureHelpParams::has_context() const { return n_.has_child("context"); }
SignatureHelpContext SignatureHelpParams::add_context() { auto w = SignatureHelpContext(n_.append_child() << ryml::key("context")); w.init(); return w; }
TextDocumentIdentifier SignatureHelpParams::textDocument() { return n_.find_child("textDocument"); }
Position SignatureHelpParams::position() { return n_.find_child("position"); }
bool SignatureHelpParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t SignatureHelpParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void SignatureHelpParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool SignatureHelpParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view SignatureHelpParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void SignatureHelpParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
SignatureHelpParams::Union3 SignatureHelpParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool SignatureHelpParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
SignatureHelpParams::Union3 SignatureHelpParams::add_workDoneToken() { auto w = SignatureHelpParams::Union3(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void SignatureHelpParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
List<SignatureInformation> SignatureHelp::signatures() { return n_.find_child("signatures"); }
std::uint32_t SignatureHelp::activeSignature() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("activeSignature")); }
bool SignatureHelp::has_activeSignature() const { return n_.has_child("activeSignature"); }
void SignatureHelp::set_activeSignature(std::uint32_t val) { auto c = n_.find_child("activeSignature"); if (c.invalid()) { c = n_.append_child() << ryml::key("activeSignature"); } detail::SetPrimitive(c, val); }
bool SignatureHelp::Union2::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t SignatureHelp::Union2::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void SignatureHelp::Union2::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
SignatureHelp::Union2 SignatureHelp::activeParameter() { return n_.find_child("activeParameter"); }
bool SignatureHelp::has_activeParameter() const { return n_.has_child("activeParameter"); }
SignatureHelp::Union2 SignatureHelp::add_activeParameter() { auto w = SignatureHelp::Union2(n_.append_child() << ryml::key("activeParameter")); w.init(); return w; }
void SignatureHelp::init() { StructWrapper::init(); List<SignatureInformation>(n_.append_child() << ryml::key("signatures")).init(); }
bool SignatureHelpRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector SignatureHelpRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
SignatureHelpRegistrationOptions::Union0 SignatureHelpRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
List<std::string_view> SignatureHelpRegistrationOptions::triggerCharacters() { return n_.find_child("triggerCharacters"); }
bool SignatureHelpRegistrationOptions::has_triggerCharacters() const { return n_.has_child("triggerCharacters"); }
List<std::string_view> SignatureHelpRegistrationOptions::add_triggerCharacters() { auto w = List<std::string_view>(n_.append_child() << ryml::key("triggerCharacters")); w.init(); return w; }
List<std::string_view> SignatureHelpRegistrationOptions::retriggerCharacters() { return n_.find_child("retriggerCharacters"); }
bool SignatureHelpRegistrationOptions::has_retriggerCharacters() const { return n_.has_child("retriggerCharacters"); }
List<std::string_view> SignatureHelpRegistrationOptions::add_retriggerCharacters() { auto w = List<std::string_view>(n_.append_child() << ryml::key("retriggerCharacters")); w.init(); return w; }
void SignatureHelpRegistrationOptions::init() { StructWrapper::init(); SignatureHelpRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier DefinitionParams::textDocument() { return n_.find_child("textDocument"); }
Position DefinitionParams::position() { return n_.find_child("position"); }
bool DefinitionParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DefinitionParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DefinitionParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DefinitionParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view DefinitionParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DefinitionParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DefinitionParams::Union2 DefinitionParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool DefinitionParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
DefinitionParams::Union2 DefinitionParams::add_workDoneToken() { auto w = DefinitionParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool DefinitionParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DefinitionParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DefinitionParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DefinitionParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view DefinitionParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DefinitionParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DefinitionParams::Union3 DefinitionParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool DefinitionParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
DefinitionParams::Union3 DefinitionParams::add_partialResultToken() { auto w = DefinitionParams::Union3(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void DefinitionParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
bool DefinitionRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector DefinitionRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
DefinitionRegistrationOptions::Union0 DefinitionRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
void DefinitionRegistrationOptions::init() { StructWrapper::init(); DefinitionRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
ReferenceContext ReferenceParams::context() { return n_.find_child("context"); }
TextDocumentIdentifier ReferenceParams::textDocument() { return n_.find_child("textDocument"); }
Position ReferenceParams::position() { return n_.find_child("position"); }
bool ReferenceParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t ReferenceParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void ReferenceParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool ReferenceParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view ReferenceParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void ReferenceParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
ReferenceParams::Union3 ReferenceParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool ReferenceParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
ReferenceParams::Union3 ReferenceParams::add_workDoneToken() { auto w = ReferenceParams::Union3(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool ReferenceParams::Union4::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t ReferenceParams::Union4::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void ReferenceParams::Union4::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool ReferenceParams::Union4::holds_string() const { return n_.has_child("TODO"); }
std::string_view ReferenceParams::Union4::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void ReferenceParams::Union4::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
ReferenceParams::Union4 ReferenceParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool ReferenceParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
ReferenceParams::Union4 ReferenceParams::add_partialResultToken() { auto w = ReferenceParams::Union4(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void ReferenceParams::init() { StructWrapper::init(); ReferenceContext(n_.append_child() << ryml::key("context")).init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
bool ReferenceRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector ReferenceRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
ReferenceRegistrationOptions::Union0 ReferenceRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
void ReferenceRegistrationOptions::init() { StructWrapper::init(); ReferenceRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier DocumentHighlightParams::textDocument() { return n_.find_child("textDocument"); }
Position DocumentHighlightParams::position() { return n_.find_child("position"); }
bool DocumentHighlightParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DocumentHighlightParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DocumentHighlightParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DocumentHighlightParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view DocumentHighlightParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DocumentHighlightParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DocumentHighlightParams::Union2 DocumentHighlightParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool DocumentHighlightParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
DocumentHighlightParams::Union2 DocumentHighlightParams::add_workDoneToken() { auto w = DocumentHighlightParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool DocumentHighlightParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DocumentHighlightParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DocumentHighlightParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DocumentHighlightParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view DocumentHighlightParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DocumentHighlightParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DocumentHighlightParams::Union3 DocumentHighlightParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool DocumentHighlightParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
DocumentHighlightParams::Union3 DocumentHighlightParams::add_partialResultToken() { auto w = DocumentHighlightParams::Union3(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void DocumentHighlightParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
Range DocumentHighlight::range() { return n_.find_child("range"); }
DocumentHighlightKind DocumentHighlight::kind() { return detail::DeserializeEnum<DocumentHighlightKind>(n_.find_child("kind")); }
bool DocumentHighlight::has_kind() const { return n_.has_child("kind"); }
void DocumentHighlight::set_kind(DocumentHighlightKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
void DocumentHighlight::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); }
bool DocumentHighlightRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector DocumentHighlightRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
DocumentHighlightRegistrationOptions::Union0 DocumentHighlightRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
void DocumentHighlightRegistrationOptions::init() { StructWrapper::init(); DocumentHighlightRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier DocumentSymbolParams::textDocument() { return n_.find_child("textDocument"); }
bool DocumentSymbolParams::Union1::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DocumentSymbolParams::Union1::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DocumentSymbolParams::Union1::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DocumentSymbolParams::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view DocumentSymbolParams::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DocumentSymbolParams::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DocumentSymbolParams::Union1 DocumentSymbolParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool DocumentSymbolParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
DocumentSymbolParams::Union1 DocumentSymbolParams::add_workDoneToken() { auto w = DocumentSymbolParams::Union1(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool DocumentSymbolParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DocumentSymbolParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DocumentSymbolParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DocumentSymbolParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view DocumentSymbolParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DocumentSymbolParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DocumentSymbolParams::Union2 DocumentSymbolParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool DocumentSymbolParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
DocumentSymbolParams::Union2 DocumentSymbolParams::add_partialResultToken() { auto w = DocumentSymbolParams::Union2(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void DocumentSymbolParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); }
bool SymbolInformation::deprecated() { return detail::NodeAsPrimitive<bool>(n_.find_child("deprecated")); }
bool SymbolInformation::has_deprecated() const { return n_.has_child("deprecated"); }
void SymbolInformation::set_deprecated(bool val) { auto c = n_.find_child("deprecated"); if (c.invalid()) { c = n_.append_child() << ryml::key("deprecated"); } detail::SetPrimitive(c, val); }
Location SymbolInformation::location() { return n_.find_child("location"); }
std::string_view SymbolInformation::name() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("name")); }
void SymbolInformation::set_name(std::string_view val) { auto c = n_.find_child("name"); if (c.invalid()) { c = n_.append_child() << ryml::key("name"); } detail::SetPrimitive(c, val); }
SymbolKind SymbolInformation::kind() { return detail::DeserializeEnum<SymbolKind>(n_.find_child("kind")); }
void SymbolInformation::set_kind(SymbolKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
List<SymbolTag> SymbolInformation::tags() { return n_.find_child("tags"); }
bool SymbolInformation::has_tags() const { return n_.has_child("tags"); }
List<SymbolTag> SymbolInformation::add_tags() { auto w = List<SymbolTag>(n_.append_child() << ryml::key("tags")); w.init(); return w; }
std::string_view SymbolInformation::containerName() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("containerName")); }
bool SymbolInformation::has_containerName() const { return n_.has_child("containerName"); }
void SymbolInformation::set_containerName(std::string_view val) { auto c = n_.find_child("containerName"); if (c.invalid()) { c = n_.append_child() << ryml::key("containerName"); } detail::SetPrimitive(c, val); }
void SymbolInformation::init() { StructWrapper::init(); Location(n_.append_child() << ryml::key("location")).init(); }
std::string_view DocumentSymbol::name() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("name")); }
void DocumentSymbol::set_name(std::string_view val) { auto c = n_.find_child("name"); if (c.invalid()) { c = n_.append_child() << ryml::key("name"); } detail::SetPrimitive(c, val); }
std::string_view DocumentSymbol::detail() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("detail")); }
bool DocumentSymbol::has_detail() const { return n_.has_child("detail"); }
void DocumentSymbol::set_detail(std::string_view val) { auto c = n_.find_child("detail"); if (c.invalid()) { c = n_.append_child() << ryml::key("detail"); } detail::SetPrimitive(c, val); }
SymbolKind DocumentSymbol::kind() { return detail::DeserializeEnum<SymbolKind>(n_.find_child("kind")); }
void DocumentSymbol::set_kind(SymbolKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
List<SymbolTag> DocumentSymbol::tags() { return n_.find_child("tags"); }
bool DocumentSymbol::has_tags() const { return n_.has_child("tags"); }
List<SymbolTag> DocumentSymbol::add_tags() { auto w = List<SymbolTag>(n_.append_child() << ryml::key("tags")); w.init(); return w; }
bool DocumentSymbol::deprecated() { return detail::NodeAsPrimitive<bool>(n_.find_child("deprecated")); }
bool DocumentSymbol::has_deprecated() const { return n_.has_child("deprecated"); }
void DocumentSymbol::set_deprecated(bool val) { auto c = n_.find_child("deprecated"); if (c.invalid()) { c = n_.append_child() << ryml::key("deprecated"); } detail::SetPrimitive(c, val); }
Range DocumentSymbol::range() { return n_.find_child("range"); }
Range DocumentSymbol::selectionRange() { return n_.find_child("selectionRange"); }
List<DocumentSymbol> DocumentSymbol::children() { return n_.find_child("children"); }
bool DocumentSymbol::has_children() const { return n_.has_child("children"); }
List<DocumentSymbol> DocumentSymbol::add_children() { auto w = List<DocumentSymbol>(n_.append_child() << ryml::key("children")); w.init(); return w; }
void DocumentSymbol::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); Range(n_.append_child() << ryml::key("selectionRange")).init(); }
bool DocumentSymbolRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector DocumentSymbolRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
DocumentSymbolRegistrationOptions::Union0 DocumentSymbolRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
std::string_view DocumentSymbolRegistrationOptions::label() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("label")); }
bool DocumentSymbolRegistrationOptions::has_label() const { return n_.has_child("label"); }
void DocumentSymbolRegistrationOptions::set_label(std::string_view val) { auto c = n_.find_child("label"); if (c.invalid()) { c = n_.append_child() << ryml::key("label"); } detail::SetPrimitive(c, val); }
void DocumentSymbolRegistrationOptions::init() { StructWrapper::init(); DocumentSymbolRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier CodeActionParams::textDocument() { return n_.find_child("textDocument"); }
Range CodeActionParams::range() { return n_.find_child("range"); }
CodeActionContext CodeActionParams::context() { return n_.find_child("context"); }
bool CodeActionParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CodeActionParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CodeActionParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CodeActionParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view CodeActionParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CodeActionParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
CodeActionParams::Union3 CodeActionParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool CodeActionParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
CodeActionParams::Union3 CodeActionParams::add_workDoneToken() { auto w = CodeActionParams::Union3(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool CodeActionParams::Union4::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CodeActionParams::Union4::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CodeActionParams::Union4::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CodeActionParams::Union4::holds_string() const { return n_.has_child("TODO"); }
std::string_view CodeActionParams::Union4::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CodeActionParams::Union4::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
CodeActionParams::Union4 CodeActionParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool CodeActionParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
CodeActionParams::Union4 CodeActionParams::add_partialResultToken() { auto w = CodeActionParams::Union4(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void CodeActionParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Range(n_.append_child() << ryml::key("range")).init(); CodeActionContext(n_.append_child() << ryml::key("context")).init(); }
std::string_view Command::title() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("title")); }
void Command::set_title(std::string_view val) { auto c = n_.find_child("title"); if (c.invalid()) { c = n_.append_child() << ryml::key("title"); } detail::SetPrimitive(c, val); }
std::string_view Command::tooltip() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("tooltip")); }
bool Command::has_tooltip() const { return n_.has_child("tooltip"); }
void Command::set_tooltip(std::string_view val) { auto c = n_.find_child("tooltip"); if (c.invalid()) { c = n_.append_child() << ryml::key("tooltip"); } detail::SetPrimitive(c, val); }
std::string_view Command::command() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("command")); }
void Command::set_command(std::string_view val) { auto c = n_.find_child("command"); if (c.invalid()) { c = n_.append_child() << ryml::key("command"); } detail::SetPrimitive(c, val); }
List<LSPAny> Command::arguments() { return n_.find_child("arguments"); }
bool Command::has_arguments() const { return n_.has_child("arguments"); }
List<LSPAny> Command::add_arguments() { auto w = List<LSPAny>(n_.append_child() << ryml::key("arguments")); w.init(); return w; }
void Command::init() { StructWrapper::init();  }
std::string_view CodeAction::title() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("title")); }
void CodeAction::set_title(std::string_view val) { auto c = n_.find_child("title"); if (c.invalid()) { c = n_.append_child() << ryml::key("title"); } detail::SetPrimitive(c, val); }
CodeActionKind CodeAction::kind() { return detail::DeserializeEnum<CodeActionKind>(n_.find_child("kind")); }
bool CodeAction::has_kind() const { return n_.has_child("kind"); }
void CodeAction::set_kind(CodeActionKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
List<Diagnostic> CodeAction::diagnostics() { return n_.find_child("diagnostics"); }
bool CodeAction::has_diagnostics() const { return n_.has_child("diagnostics"); }
List<Diagnostic> CodeAction::add_diagnostics() { auto w = List<Diagnostic>(n_.append_child() << ryml::key("diagnostics")); w.init(); return w; }
bool CodeAction::isPreferred() { return detail::NodeAsPrimitive<bool>(n_.find_child("isPreferred")); }
bool CodeAction::has_isPreferred() const { return n_.has_child("isPreferred"); }
void CodeAction::set_isPreferred(bool val) { auto c = n_.find_child("isPreferred"); if (c.invalid()) { c = n_.append_child() << ryml::key("isPreferred"); } detail::SetPrimitive(c, val); }
CodeActionDisabled CodeAction::disabled() { return n_.find_child("disabled"); }
bool CodeAction::has_disabled() const { return n_.has_child("disabled"); }
CodeActionDisabled CodeAction::add_disabled() { auto w = CodeActionDisabled(n_.append_child() << ryml::key("disabled")); w.init(); return w; }
WorkspaceEdit CodeAction::edit() { return n_.find_child("edit"); }
bool CodeAction::has_edit() const { return n_.has_child("edit"); }
WorkspaceEdit CodeAction::add_edit() { auto w = WorkspaceEdit(n_.append_child() << ryml::key("edit")); w.init(); return w; }
Command CodeAction::command() { return n_.find_child("command"); }
bool CodeAction::has_command() const { return n_.has_child("command"); }
Command CodeAction::add_command() { auto w = Command(n_.append_child() << ryml::key("command")); w.init(); return w; }
bool CodeAction::Union7::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject CodeAction::Union7::as_reference_Map0() {  }
bool CodeAction::Union7::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray CodeAction::Union7::as_reference_LSPAny() { return {n_}; }
bool CodeAction::Union7::holds_string() const { return n_.has_child("TODO"); }
std::string_view CodeAction::Union7::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CodeAction::Union7::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool CodeAction::Union7::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CodeAction::Union7::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CodeAction::Union7::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CodeAction::Union7::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t CodeAction::Union7::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void CodeAction::Union7::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool CodeAction::Union7::holds_decimal() const { return n_.has_child("TODO"); }
double CodeAction::Union7::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void CodeAction::Union7::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool CodeAction::Union7::holds_boolean() const { return n_.has_child("TODO"); }
bool CodeAction::Union7::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void CodeAction::Union7::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
CodeAction::Union7 CodeAction::data() { return n_.find_child("data"); }
bool CodeAction::has_data() const { return n_.has_child("data"); }
CodeAction::Union7 CodeAction::add_data() { auto w = CodeAction::Union7(n_.append_child() << ryml::key("data")); w.init(); return w; }
List<CodeActionTag> CodeAction::tags() { return n_.find_child("tags"); }
bool CodeAction::has_tags() const { return n_.has_child("tags"); }
List<CodeActionTag> CodeAction::add_tags() { auto w = List<CodeActionTag>(n_.append_child() << ryml::key("tags")); w.init(); return w; }
void CodeAction::init() { StructWrapper::init();  }
bool CodeActionRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector CodeActionRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
CodeActionRegistrationOptions::Union0 CodeActionRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
List<CodeActionKind> CodeActionRegistrationOptions::codeActionKinds() { return n_.find_child("codeActionKinds"); }
bool CodeActionRegistrationOptions::has_codeActionKinds() const { return n_.has_child("codeActionKinds"); }
List<CodeActionKind> CodeActionRegistrationOptions::add_codeActionKinds() { auto w = List<CodeActionKind>(n_.append_child() << ryml::key("codeActionKinds")); w.init(); return w; }
List<CodeActionKindDocumentation> CodeActionRegistrationOptions::documentation() { return n_.find_child("documentation"); }
bool CodeActionRegistrationOptions::has_documentation() const { return n_.has_child("documentation"); }
List<CodeActionKindDocumentation> CodeActionRegistrationOptions::add_documentation() { auto w = List<CodeActionKindDocumentation>(n_.append_child() << ryml::key("documentation")); w.init(); return w; }
bool CodeActionRegistrationOptions::resolveProvider() { return detail::NodeAsPrimitive<bool>(n_.find_child("resolveProvider")); }
bool CodeActionRegistrationOptions::has_resolveProvider() const { return n_.has_child("resolveProvider"); }
void CodeActionRegistrationOptions::set_resolveProvider(bool val) { auto c = n_.find_child("resolveProvider"); if (c.invalid()) { c = n_.append_child() << ryml::key("resolveProvider"); } detail::SetPrimitive(c, val); }
void CodeActionRegistrationOptions::init() { StructWrapper::init(); CodeActionRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
std::string_view WorkspaceSymbolParams::query() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("query")); }
void WorkspaceSymbolParams::set_query(std::string_view val) { auto c = n_.find_child("query"); if (c.invalid()) { c = n_.append_child() << ryml::key("query"); } detail::SetPrimitive(c, val); }
bool WorkspaceSymbolParams::Union1::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t WorkspaceSymbolParams::Union1::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void WorkspaceSymbolParams::Union1::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool WorkspaceSymbolParams::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view WorkspaceSymbolParams::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void WorkspaceSymbolParams::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
WorkspaceSymbolParams::Union1 WorkspaceSymbolParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool WorkspaceSymbolParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
WorkspaceSymbolParams::Union1 WorkspaceSymbolParams::add_workDoneToken() { auto w = WorkspaceSymbolParams::Union1(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool WorkspaceSymbolParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t WorkspaceSymbolParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void WorkspaceSymbolParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool WorkspaceSymbolParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view WorkspaceSymbolParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void WorkspaceSymbolParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
WorkspaceSymbolParams::Union2 WorkspaceSymbolParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool WorkspaceSymbolParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
WorkspaceSymbolParams::Union2 WorkspaceSymbolParams::add_partialResultToken() { auto w = WorkspaceSymbolParams::Union2(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void WorkspaceSymbolParams::init() { StructWrapper::init();  }
bool WorkspaceSymbol::Union0::holds_Location() const { return n_.has_child("TODO"); }
Location WorkspaceSymbol::Union0::as_Location() { return {n_}; }
bool WorkspaceSymbol::Union0::holds_LocationUriOnly() const { return n_.has_child("TODO"); }
LocationUriOnly WorkspaceSymbol::Union0::as_LocationUriOnly() { return {n_}; }
WorkspaceSymbol::Union0 WorkspaceSymbol::location() { return n_.find_child("location"); }
bool WorkspaceSymbol::Union1::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject WorkspaceSymbol::Union1::as_reference_Map0() {  }
bool WorkspaceSymbol::Union1::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray WorkspaceSymbol::Union1::as_reference_LSPAny() { return {n_}; }
bool WorkspaceSymbol::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view WorkspaceSymbol::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void WorkspaceSymbol::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool WorkspaceSymbol::Union1::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t WorkspaceSymbol::Union1::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void WorkspaceSymbol::Union1::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool WorkspaceSymbol::Union1::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t WorkspaceSymbol::Union1::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void WorkspaceSymbol::Union1::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool WorkspaceSymbol::Union1::holds_decimal() const { return n_.has_child("TODO"); }
double WorkspaceSymbol::Union1::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void WorkspaceSymbol::Union1::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool WorkspaceSymbol::Union1::holds_boolean() const { return n_.has_child("TODO"); }
bool WorkspaceSymbol::Union1::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void WorkspaceSymbol::Union1::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
WorkspaceSymbol::Union1 WorkspaceSymbol::data() { return n_.find_child("data"); }
bool WorkspaceSymbol::has_data() const { return n_.has_child("data"); }
WorkspaceSymbol::Union1 WorkspaceSymbol::add_data() { auto w = WorkspaceSymbol::Union1(n_.append_child() << ryml::key("data")); w.init(); return w; }
std::string_view WorkspaceSymbol::name() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("name")); }
void WorkspaceSymbol::set_name(std::string_view val) { auto c = n_.find_child("name"); if (c.invalid()) { c = n_.append_child() << ryml::key("name"); } detail::SetPrimitive(c, val); }
SymbolKind WorkspaceSymbol::kind() { return detail::DeserializeEnum<SymbolKind>(n_.find_child("kind")); }
void WorkspaceSymbol::set_kind(SymbolKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
List<SymbolTag> WorkspaceSymbol::tags() { return n_.find_child("tags"); }
bool WorkspaceSymbol::has_tags() const { return n_.has_child("tags"); }
List<SymbolTag> WorkspaceSymbol::add_tags() { auto w = List<SymbolTag>(n_.append_child() << ryml::key("tags")); w.init(); return w; }
std::string_view WorkspaceSymbol::containerName() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("containerName")); }
bool WorkspaceSymbol::has_containerName() const { return n_.has_child("containerName"); }
void WorkspaceSymbol::set_containerName(std::string_view val) { auto c = n_.find_child("containerName"); if (c.invalid()) { c = n_.append_child() << ryml::key("containerName"); } detail::SetPrimitive(c, val); }
void WorkspaceSymbol::init() { StructWrapper::init(); WorkspaceSymbol::Union0(n_.append_child() << ryml::key("location")).init(); }
bool WorkspaceSymbolRegistrationOptions::resolveProvider() { return detail::NodeAsPrimitive<bool>(n_.find_child("resolveProvider")); }
bool WorkspaceSymbolRegistrationOptions::has_resolveProvider() const { return n_.has_child("resolveProvider"); }
void WorkspaceSymbolRegistrationOptions::set_resolveProvider(bool val) { auto c = n_.find_child("resolveProvider"); if (c.invalid()) { c = n_.append_child() << ryml::key("resolveProvider"); } detail::SetPrimitive(c, val); }
void WorkspaceSymbolRegistrationOptions::init() { StructWrapper::init();  }
TextDocumentIdentifier CodeLensParams::textDocument() { return n_.find_child("textDocument"); }
bool CodeLensParams::Union1::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CodeLensParams::Union1::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CodeLensParams::Union1::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CodeLensParams::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view CodeLensParams::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CodeLensParams::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
CodeLensParams::Union1 CodeLensParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool CodeLensParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
CodeLensParams::Union1 CodeLensParams::add_workDoneToken() { auto w = CodeLensParams::Union1(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool CodeLensParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CodeLensParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CodeLensParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CodeLensParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view CodeLensParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CodeLensParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
CodeLensParams::Union2 CodeLensParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool CodeLensParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
CodeLensParams::Union2 CodeLensParams::add_partialResultToken() { auto w = CodeLensParams::Union2(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void CodeLensParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); }
Range CodeLens::range() { return n_.find_child("range"); }
Command CodeLens::command() { return n_.find_child("command"); }
bool CodeLens::has_command() const { return n_.has_child("command"); }
Command CodeLens::add_command() { auto w = Command(n_.append_child() << ryml::key("command")); w.init(); return w; }
bool CodeLens::Union2::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject CodeLens::Union2::as_reference_Map0() {  }
bool CodeLens::Union2::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray CodeLens::Union2::as_reference_LSPAny() { return {n_}; }
bool CodeLens::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view CodeLens::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CodeLens::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool CodeLens::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CodeLens::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CodeLens::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CodeLens::Union2::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t CodeLens::Union2::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void CodeLens::Union2::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool CodeLens::Union2::holds_decimal() const { return n_.has_child("TODO"); }
double CodeLens::Union2::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void CodeLens::Union2::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool CodeLens::Union2::holds_boolean() const { return n_.has_child("TODO"); }
bool CodeLens::Union2::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void CodeLens::Union2::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
CodeLens::Union2 CodeLens::data() { return n_.find_child("data"); }
bool CodeLens::has_data() const { return n_.has_child("data"); }
CodeLens::Union2 CodeLens::add_data() { auto w = CodeLens::Union2(n_.append_child() << ryml::key("data")); w.init(); return w; }
void CodeLens::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); }
bool CodeLensRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector CodeLensRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
CodeLensRegistrationOptions::Union0 CodeLensRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
bool CodeLensRegistrationOptions::resolveProvider() { return detail::NodeAsPrimitive<bool>(n_.find_child("resolveProvider")); }
bool CodeLensRegistrationOptions::has_resolveProvider() const { return n_.has_child("resolveProvider"); }
void CodeLensRegistrationOptions::set_resolveProvider(bool val) { auto c = n_.find_child("resolveProvider"); if (c.invalid()) { c = n_.append_child() << ryml::key("resolveProvider"); } detail::SetPrimitive(c, val); }
void CodeLensRegistrationOptions::init() { StructWrapper::init(); CodeLensRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier DocumentLinkParams::textDocument() { return n_.find_child("textDocument"); }
bool DocumentLinkParams::Union1::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DocumentLinkParams::Union1::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DocumentLinkParams::Union1::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DocumentLinkParams::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view DocumentLinkParams::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DocumentLinkParams::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DocumentLinkParams::Union1 DocumentLinkParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool DocumentLinkParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
DocumentLinkParams::Union1 DocumentLinkParams::add_workDoneToken() { auto w = DocumentLinkParams::Union1(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
bool DocumentLinkParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DocumentLinkParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DocumentLinkParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DocumentLinkParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view DocumentLinkParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DocumentLinkParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DocumentLinkParams::Union2 DocumentLinkParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool DocumentLinkParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
DocumentLinkParams::Union2 DocumentLinkParams::add_partialResultToken() { auto w = DocumentLinkParams::Union2(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void DocumentLinkParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); }
Range DocumentLink::range() { return n_.find_child("range"); }
std::string_view DocumentLink::target() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("target")); }
bool DocumentLink::has_target() const { return n_.has_child("target"); }
void DocumentLink::set_target(std::string_view val) { auto c = n_.find_child("target"); if (c.invalid()) { c = n_.append_child() << ryml::key("target"); } detail::SetPrimitive(c, val); }
std::string_view DocumentLink::tooltip() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("tooltip")); }
bool DocumentLink::has_tooltip() const { return n_.has_child("tooltip"); }
void DocumentLink::set_tooltip(std::string_view val) { auto c = n_.find_child("tooltip"); if (c.invalid()) { c = n_.append_child() << ryml::key("tooltip"); } detail::SetPrimitive(c, val); }
bool DocumentLink::Union3::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject DocumentLink::Union3::as_reference_Map0() {  }
bool DocumentLink::Union3::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray DocumentLink::Union3::as_reference_LSPAny() { return {n_}; }
bool DocumentLink::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view DocumentLink::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DocumentLink::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool DocumentLink::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DocumentLink::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DocumentLink::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DocumentLink::Union3::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t DocumentLink::Union3::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void DocumentLink::Union3::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool DocumentLink::Union3::holds_decimal() const { return n_.has_child("TODO"); }
double DocumentLink::Union3::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void DocumentLink::Union3::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool DocumentLink::Union3::holds_boolean() const { return n_.has_child("TODO"); }
bool DocumentLink::Union3::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void DocumentLink::Union3::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
DocumentLink::Union3 DocumentLink::data() { return n_.find_child("data"); }
bool DocumentLink::has_data() const { return n_.has_child("data"); }
DocumentLink::Union3 DocumentLink::add_data() { auto w = DocumentLink::Union3(n_.append_child() << ryml::key("data")); w.init(); return w; }
void DocumentLink::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); }
bool DocumentLinkRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector DocumentLinkRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
DocumentLinkRegistrationOptions::Union0 DocumentLinkRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
bool DocumentLinkRegistrationOptions::resolveProvider() { return detail::NodeAsPrimitive<bool>(n_.find_child("resolveProvider")); }
bool DocumentLinkRegistrationOptions::has_resolveProvider() const { return n_.has_child("resolveProvider"); }
void DocumentLinkRegistrationOptions::set_resolveProvider(bool val) { auto c = n_.find_child("resolveProvider"); if (c.invalid()) { c = n_.append_child() << ryml::key("resolveProvider"); } detail::SetPrimitive(c, val); }
void DocumentLinkRegistrationOptions::init() { StructWrapper::init(); DocumentLinkRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier DocumentFormattingParams::textDocument() { return n_.find_child("textDocument"); }
FormattingOptions DocumentFormattingParams::options() { return n_.find_child("options"); }
bool DocumentFormattingParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DocumentFormattingParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DocumentFormattingParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DocumentFormattingParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view DocumentFormattingParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DocumentFormattingParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DocumentFormattingParams::Union2 DocumentFormattingParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool DocumentFormattingParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
DocumentFormattingParams::Union2 DocumentFormattingParams::add_workDoneToken() { auto w = DocumentFormattingParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void DocumentFormattingParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); FormattingOptions(n_.append_child() << ryml::key("options")).init(); }
bool DocumentFormattingRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector DocumentFormattingRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
DocumentFormattingRegistrationOptions::Union0 DocumentFormattingRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
void DocumentFormattingRegistrationOptions::init() { StructWrapper::init(); DocumentFormattingRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier DocumentRangeFormattingParams::textDocument() { return n_.find_child("textDocument"); }
Range DocumentRangeFormattingParams::range() { return n_.find_child("range"); }
FormattingOptions DocumentRangeFormattingParams::options() { return n_.find_child("options"); }
bool DocumentRangeFormattingParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DocumentRangeFormattingParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DocumentRangeFormattingParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DocumentRangeFormattingParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view DocumentRangeFormattingParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DocumentRangeFormattingParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DocumentRangeFormattingParams::Union3 DocumentRangeFormattingParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool DocumentRangeFormattingParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
DocumentRangeFormattingParams::Union3 DocumentRangeFormattingParams::add_workDoneToken() { auto w = DocumentRangeFormattingParams::Union3(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void DocumentRangeFormattingParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Range(n_.append_child() << ryml::key("range")).init(); FormattingOptions(n_.append_child() << ryml::key("options")).init(); }
bool DocumentRangeFormattingRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector DocumentRangeFormattingRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
DocumentRangeFormattingRegistrationOptions::Union0 DocumentRangeFormattingRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
bool DocumentRangeFormattingRegistrationOptions::rangesSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("rangesSupport")); }
bool DocumentRangeFormattingRegistrationOptions::has_rangesSupport() const { return n_.has_child("rangesSupport"); }
void DocumentRangeFormattingRegistrationOptions::set_rangesSupport(bool val) { auto c = n_.find_child("rangesSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("rangesSupport"); } detail::SetPrimitive(c, val); }
void DocumentRangeFormattingRegistrationOptions::init() { StructWrapper::init(); DocumentRangeFormattingRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier DocumentRangesFormattingParams::textDocument() { return n_.find_child("textDocument"); }
List<Range> DocumentRangesFormattingParams::ranges() { return n_.find_child("ranges"); }
FormattingOptions DocumentRangesFormattingParams::options() { return n_.find_child("options"); }
bool DocumentRangesFormattingParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t DocumentRangesFormattingParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void DocumentRangesFormattingParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool DocumentRangesFormattingParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view DocumentRangesFormattingParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void DocumentRangesFormattingParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
DocumentRangesFormattingParams::Union3 DocumentRangesFormattingParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool DocumentRangesFormattingParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
DocumentRangesFormattingParams::Union3 DocumentRangesFormattingParams::add_workDoneToken() { auto w = DocumentRangesFormattingParams::Union3(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void DocumentRangesFormattingParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); List<Range>(n_.append_child() << ryml::key("ranges")).init(); FormattingOptions(n_.append_child() << ryml::key("options")).init(); }
TextDocumentIdentifier DocumentOnTypeFormattingParams::textDocument() { return n_.find_child("textDocument"); }
Position DocumentOnTypeFormattingParams::position() { return n_.find_child("position"); }
std::string_view DocumentOnTypeFormattingParams::ch() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("ch")); }
void DocumentOnTypeFormattingParams::set_ch(std::string_view val) { auto c = n_.find_child("ch"); if (c.invalid()) { c = n_.append_child() << ryml::key("ch"); } detail::SetPrimitive(c, val); }
FormattingOptions DocumentOnTypeFormattingParams::options() { return n_.find_child("options"); }
void DocumentOnTypeFormattingParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); FormattingOptions(n_.append_child() << ryml::key("options")).init(); }
bool DocumentOnTypeFormattingRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector DocumentOnTypeFormattingRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
DocumentOnTypeFormattingRegistrationOptions::Union0 DocumentOnTypeFormattingRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
std::string_view DocumentOnTypeFormattingRegistrationOptions::firstTriggerCharacter() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("firstTriggerCharacter")); }
void DocumentOnTypeFormattingRegistrationOptions::set_firstTriggerCharacter(std::string_view val) { auto c = n_.find_child("firstTriggerCharacter"); if (c.invalid()) { c = n_.append_child() << ryml::key("firstTriggerCharacter"); } detail::SetPrimitive(c, val); }
List<std::string_view> DocumentOnTypeFormattingRegistrationOptions::moreTriggerCharacter() { return n_.find_child("moreTriggerCharacter"); }
bool DocumentOnTypeFormattingRegistrationOptions::has_moreTriggerCharacter() const { return n_.has_child("moreTriggerCharacter"); }
List<std::string_view> DocumentOnTypeFormattingRegistrationOptions::add_moreTriggerCharacter() { auto w = List<std::string_view>(n_.append_child() << ryml::key("moreTriggerCharacter")); w.init(); return w; }
void DocumentOnTypeFormattingRegistrationOptions::init() { StructWrapper::init(); DocumentOnTypeFormattingRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier RenameParams::textDocument() { return n_.find_child("textDocument"); }
Position RenameParams::position() { return n_.find_child("position"); }
std::string_view RenameParams::newName() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("newName")); }
void RenameParams::set_newName(std::string_view val) { auto c = n_.find_child("newName"); if (c.invalid()) { c = n_.append_child() << ryml::key("newName"); } detail::SetPrimitive(c, val); }
bool RenameParams::Union3::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t RenameParams::Union3::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void RenameParams::Union3::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool RenameParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view RenameParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void RenameParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
RenameParams::Union3 RenameParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool RenameParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
RenameParams::Union3 RenameParams::add_workDoneToken() { auto w = RenameParams::Union3(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void RenameParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
bool RenameRegistrationOptions::Union0::holds_reference_DocumentFilter() const { return n_.has_child("TODO"); }
DocumentSelector RenameRegistrationOptions::Union0::as_reference_DocumentFilter() { return {n_}; }
RenameRegistrationOptions::Union0 RenameRegistrationOptions::documentSelector() { return n_.find_child("documentSelector"); }
bool RenameRegistrationOptions::prepareProvider() { return detail::NodeAsPrimitive<bool>(n_.find_child("prepareProvider")); }
bool RenameRegistrationOptions::has_prepareProvider() const { return n_.has_child("prepareProvider"); }
void RenameRegistrationOptions::set_prepareProvider(bool val) { auto c = n_.find_child("prepareProvider"); if (c.invalid()) { c = n_.append_child() << ryml::key("prepareProvider"); } detail::SetPrimitive(c, val); }
void RenameRegistrationOptions::init() { StructWrapper::init(); RenameRegistrationOptions::Union0(n_.append_child() << ryml::key("documentSelector")).init(); }
TextDocumentIdentifier PrepareRenameParams::textDocument() { return n_.find_child("textDocument"); }
Position PrepareRenameParams::position() { return n_.find_child("position"); }
bool PrepareRenameParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t PrepareRenameParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void PrepareRenameParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool PrepareRenameParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view PrepareRenameParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void PrepareRenameParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
PrepareRenameParams::Union2 PrepareRenameParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool PrepareRenameParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
PrepareRenameParams::Union2 PrepareRenameParams::add_workDoneToken() { auto w = PrepareRenameParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void PrepareRenameParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
std::string_view ExecuteCommandParams::command() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("command")); }
void ExecuteCommandParams::set_command(std::string_view val) { auto c = n_.find_child("command"); if (c.invalid()) { c = n_.append_child() << ryml::key("command"); } detail::SetPrimitive(c, val); }
List<LSPAny> ExecuteCommandParams::arguments() { return n_.find_child("arguments"); }
bool ExecuteCommandParams::has_arguments() const { return n_.has_child("arguments"); }
List<LSPAny> ExecuteCommandParams::add_arguments() { auto w = List<LSPAny>(n_.append_child() << ryml::key("arguments")); w.init(); return w; }
bool ExecuteCommandParams::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t ExecuteCommandParams::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void ExecuteCommandParams::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool ExecuteCommandParams::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view ExecuteCommandParams::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void ExecuteCommandParams::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
ExecuteCommandParams::Union2 ExecuteCommandParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool ExecuteCommandParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
ExecuteCommandParams::Union2 ExecuteCommandParams::add_workDoneToken() { auto w = ExecuteCommandParams::Union2(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void ExecuteCommandParams::init() { StructWrapper::init();  }
List<std::string_view> ExecuteCommandRegistrationOptions::commands() { return n_.find_child("commands"); }
void ExecuteCommandRegistrationOptions::init() { StructWrapper::init(); List<std::string_view>(n_.append_child() << ryml::key("commands")).init(); }
std::string_view ApplyWorkspaceEditParams::label() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("label")); }
bool ApplyWorkspaceEditParams::has_label() const { return n_.has_child("label"); }
void ApplyWorkspaceEditParams::set_label(std::string_view val) { auto c = n_.find_child("label"); if (c.invalid()) { c = n_.append_child() << ryml::key("label"); } detail::SetPrimitive(c, val); }
WorkspaceEdit ApplyWorkspaceEditParams::edit() { return n_.find_child("edit"); }
WorkspaceEditMetadata ApplyWorkspaceEditParams::metadata() { return n_.find_child("metadata"); }
bool ApplyWorkspaceEditParams::has_metadata() const { return n_.has_child("metadata"); }
WorkspaceEditMetadata ApplyWorkspaceEditParams::add_metadata() { auto w = WorkspaceEditMetadata(n_.append_child() << ryml::key("metadata")); w.init(); return w; }
void ApplyWorkspaceEditParams::init() { StructWrapper::init(); WorkspaceEdit(n_.append_child() << ryml::key("edit")).init(); }
bool ApplyWorkspaceEditResult::applied() { return detail::NodeAsPrimitive<bool>(n_.find_child("applied")); }
void ApplyWorkspaceEditResult::set_applied(bool val) { auto c = n_.find_child("applied"); if (c.invalid()) { c = n_.append_child() << ryml::key("applied"); } detail::SetPrimitive(c, val); }
std::string_view ApplyWorkspaceEditResult::failureReason() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("failureReason")); }
bool ApplyWorkspaceEditResult::has_failureReason() const { return n_.has_child("failureReason"); }
void ApplyWorkspaceEditResult::set_failureReason(std::string_view val) { auto c = n_.find_child("failureReason"); if (c.invalid()) { c = n_.append_child() << ryml::key("failureReason"); } detail::SetPrimitive(c, val); }
std::uint32_t ApplyWorkspaceEditResult::failedChange() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("failedChange")); }
bool ApplyWorkspaceEditResult::has_failedChange() const { return n_.has_child("failedChange"); }
void ApplyWorkspaceEditResult::set_failedChange(std::uint32_t val) { auto c = n_.find_child("failedChange"); if (c.invalid()) { c = n_.append_child() << ryml::key("failedChange"); } detail::SetPrimitive(c, val); }
void ApplyWorkspaceEditResult::init() { StructWrapper::init();  }
std::string_view WorkDoneProgressBegin::title() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("title")); }
void WorkDoneProgressBegin::set_title(std::string_view val) { auto c = n_.find_child("title"); if (c.invalid()) { c = n_.append_child() << ryml::key("title"); } detail::SetPrimitive(c, val); }
bool WorkDoneProgressBegin::cancellable() { return detail::NodeAsPrimitive<bool>(n_.find_child("cancellable")); }
bool WorkDoneProgressBegin::has_cancellable() const { return n_.has_child("cancellable"); }
void WorkDoneProgressBegin::set_cancellable(bool val) { auto c = n_.find_child("cancellable"); if (c.invalid()) { c = n_.append_child() << ryml::key("cancellable"); } detail::SetPrimitive(c, val); }
std::string_view WorkDoneProgressBegin::message() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("message")); }
bool WorkDoneProgressBegin::has_message() const { return n_.has_child("message"); }
void WorkDoneProgressBegin::set_message(std::string_view val) { auto c = n_.find_child("message"); if (c.invalid()) { c = n_.append_child() << ryml::key("message"); } detail::SetPrimitive(c, val); }
std::uint32_t WorkDoneProgressBegin::percentage() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("percentage")); }
bool WorkDoneProgressBegin::has_percentage() const { return n_.has_child("percentage"); }
void WorkDoneProgressBegin::set_percentage(std::uint32_t val) { auto c = n_.find_child("percentage"); if (c.invalid()) { c = n_.append_child() << ryml::key("percentage"); } detail::SetPrimitive(c, val); }
void WorkDoneProgressBegin::init() { StructWrapper::init();  }
bool WorkDoneProgressReport::cancellable() { return detail::NodeAsPrimitive<bool>(n_.find_child("cancellable")); }
bool WorkDoneProgressReport::has_cancellable() const { return n_.has_child("cancellable"); }
void WorkDoneProgressReport::set_cancellable(bool val) { auto c = n_.find_child("cancellable"); if (c.invalid()) { c = n_.append_child() << ryml::key("cancellable"); } detail::SetPrimitive(c, val); }
std::string_view WorkDoneProgressReport::message() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("message")); }
bool WorkDoneProgressReport::has_message() const { return n_.has_child("message"); }
void WorkDoneProgressReport::set_message(std::string_view val) { auto c = n_.find_child("message"); if (c.invalid()) { c = n_.append_child() << ryml::key("message"); } detail::SetPrimitive(c, val); }
std::uint32_t WorkDoneProgressReport::percentage() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("percentage")); }
bool WorkDoneProgressReport::has_percentage() const { return n_.has_child("percentage"); }
void WorkDoneProgressReport::set_percentage(std::uint32_t val) { auto c = n_.find_child("percentage"); if (c.invalid()) { c = n_.append_child() << ryml::key("percentage"); } detail::SetPrimitive(c, val); }
void WorkDoneProgressReport::init() { StructWrapper::init();  }
std::string_view WorkDoneProgressEnd::message() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("message")); }
bool WorkDoneProgressEnd::has_message() const { return n_.has_child("message"); }
void WorkDoneProgressEnd::set_message(std::string_view val) { auto c = n_.find_child("message"); if (c.invalid()) { c = n_.append_child() << ryml::key("message"); } detail::SetPrimitive(c, val); }
void WorkDoneProgressEnd::init() { StructWrapper::init();  }
TraceValue SetTraceParams::value() { return detail::DeserializeEnum<TraceValue>(n_.find_child("value")); }
void SetTraceParams::set_value(TraceValue val) { auto c = n_.find_child("value"); if (c.invalid()) { c = n_.append_child() << ryml::key("value"); } detail::SerializeEnum(val, c); }
void SetTraceParams::init() { StructWrapper::init();  }
std::string_view LogTraceParams::message() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("message")); }
void LogTraceParams::set_message(std::string_view val) { auto c = n_.find_child("message"); if (c.invalid()) { c = n_.append_child() << ryml::key("message"); } detail::SetPrimitive(c, val); }
std::string_view LogTraceParams::verbose() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("verbose")); }
bool LogTraceParams::has_verbose() const { return n_.has_child("verbose"); }
void LogTraceParams::set_verbose(std::string_view val) { auto c = n_.find_child("verbose"); if (c.invalid()) { c = n_.append_child() << ryml::key("verbose"); } detail::SetPrimitive(c, val); }
void LogTraceParams::init() { StructWrapper::init();  }
bool CancelParams::Union0::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CancelParams::Union0::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CancelParams::Union0::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CancelParams::Union0::holds_string() const { return n_.has_child("TODO"); }
std::string_view CancelParams::Union0::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CancelParams::Union0::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
CancelParams::Union0 CancelParams::id() { return n_.find_child("id"); }
void CancelParams::init() { StructWrapper::init(); CancelParams::Union0(n_.append_child() << ryml::key("id")).init(); }
bool ProgressParams::Union0::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t ProgressParams::Union0::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void ProgressParams::Union0::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool ProgressParams::Union0::holds_string() const { return n_.has_child("TODO"); }
std::string_view ProgressParams::Union0::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void ProgressParams::Union0::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
ProgressParams::Union0 ProgressParams::token() { return n_.find_child("token"); }
bool ProgressParams::Union1::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject ProgressParams::Union1::as_reference_Map0() {  }
bool ProgressParams::Union1::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray ProgressParams::Union1::as_reference_LSPAny() { return {n_}; }
bool ProgressParams::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view ProgressParams::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void ProgressParams::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool ProgressParams::Union1::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t ProgressParams::Union1::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void ProgressParams::Union1::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool ProgressParams::Union1::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t ProgressParams::Union1::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void ProgressParams::Union1::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool ProgressParams::Union1::holds_decimal() const { return n_.has_child("TODO"); }
double ProgressParams::Union1::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void ProgressParams::Union1::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool ProgressParams::Union1::holds_boolean() const { return n_.has_child("TODO"); }
bool ProgressParams::Union1::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ProgressParams::Union1::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
ProgressParams::Union1 ProgressParams::value() { return n_.find_child("value"); }
void ProgressParams::init() { StructWrapper::init(); ProgressParams::Union0(n_.append_child() << ryml::key("token")).init(); ProgressParams::Union1(n_.append_child() << ryml::key("value")).init(); }
TextDocumentIdentifier TextDocumentPositionParams::textDocument() { return n_.find_child("textDocument"); }
Position TextDocumentPositionParams::position() { return n_.find_child("position"); }
void TextDocumentPositionParams::init() { StructWrapper::init(); TextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); Position(n_.append_child() << ryml::key("position")).init(); }
bool WorkDoneProgressParams::Union0::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t WorkDoneProgressParams::Union0::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void WorkDoneProgressParams::Union0::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool WorkDoneProgressParams::Union0::holds_string() const { return n_.has_child("TODO"); }
std::string_view WorkDoneProgressParams::Union0::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void WorkDoneProgressParams::Union0::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
WorkDoneProgressParams::Union0 WorkDoneProgressParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool WorkDoneProgressParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
WorkDoneProgressParams::Union0 WorkDoneProgressParams::add_workDoneToken() { auto w = WorkDoneProgressParams::Union0(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void WorkDoneProgressParams::init() { StructWrapper::init();  }
bool PartialResultParams::Union0::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t PartialResultParams::Union0::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void PartialResultParams::Union0::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool PartialResultParams::Union0::holds_string() const { return n_.has_child("TODO"); }
std::string_view PartialResultParams::Union0::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void PartialResultParams::Union0::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
PartialResultParams::Union0 PartialResultParams::partialResultToken() { return n_.find_child("partialResultToken"); }
bool PartialResultParams::has_partialResultToken() const { return n_.has_child("partialResultToken"); }
PartialResultParams::Union0 PartialResultParams::add_partialResultToken() { auto w = PartialResultParams::Union0(n_.append_child() << ryml::key("partialResultToken")); w.init(); return w; }
void PartialResultParams::init() { StructWrapper::init();  }
Range LocationLink::originSelectionRange() { return n_.find_child("originSelectionRange"); }
bool LocationLink::has_originSelectionRange() const { return n_.has_child("originSelectionRange"); }
Range LocationLink::add_originSelectionRange() { auto w = Range(n_.append_child() << ryml::key("originSelectionRange")); w.init(); return w; }
std::string_view LocationLink::targetUri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("targetUri")); }
void LocationLink::set_targetUri(std::string_view val) { auto c = n_.find_child("targetUri"); if (c.invalid()) { c = n_.append_child() << ryml::key("targetUri"); } detail::SetPrimitive(c, val); }
Range LocationLink::targetRange() { return n_.find_child("targetRange"); }
Range LocationLink::targetSelectionRange() { return n_.find_child("targetSelectionRange"); }
void LocationLink::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("targetRange")).init(); Range(n_.append_child() << ryml::key("targetSelectionRange")).init(); }
Position Range::start() { return n_.find_child("start"); }
Position Range::end() { return n_.find_child("end"); }
void Range::init() { StructWrapper::init(); Position(n_.append_child() << ryml::key("start")).init(); Position(n_.append_child() << ryml::key("end")).init(); }
bool ImplementationOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool ImplementationOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void ImplementationOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void ImplementationOptions::init() { StructWrapper::init();  }
std::string_view StaticRegistrationOptions::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
bool StaticRegistrationOptions::has_id() const { return n_.has_child("id"); }
void StaticRegistrationOptions::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
void StaticRegistrationOptions::init() { StructWrapper::init();  }
bool TypeDefinitionOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool TypeDefinitionOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void TypeDefinitionOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void TypeDefinitionOptions::init() { StructWrapper::init();  }
List<WorkspaceFolder> WorkspaceFoldersChangeEvent::added() { return n_.find_child("added"); }
List<WorkspaceFolder> WorkspaceFoldersChangeEvent::removed() { return n_.find_child("removed"); }
void WorkspaceFoldersChangeEvent::init() { StructWrapper::init(); List<WorkspaceFolder>(n_.append_child() << ryml::key("added")).init(); List<WorkspaceFolder>(n_.append_child() << ryml::key("removed")).init(); }
std::string_view ConfigurationItem::scopeUri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("scopeUri")); }
bool ConfigurationItem::has_scopeUri() const { return n_.has_child("scopeUri"); }
void ConfigurationItem::set_scopeUri(std::string_view val) { auto c = n_.find_child("scopeUri"); if (c.invalid()) { c = n_.append_child() << ryml::key("scopeUri"); } detail::SetPrimitive(c, val); }
std::string_view ConfigurationItem::section() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("section")); }
bool ConfigurationItem::has_section() const { return n_.has_child("section"); }
void ConfigurationItem::set_section(std::string_view val) { auto c = n_.find_child("section"); if (c.invalid()) { c = n_.append_child() << ryml::key("section"); } detail::SetPrimitive(c, val); }
void ConfigurationItem::init() { StructWrapper::init();  }
std::string_view TextDocumentIdentifier::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void TextDocumentIdentifier::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
void TextDocumentIdentifier::init() { StructWrapper::init();  }
double Color::red() { return detail::NodeAsPrimitive<double>(n_.find_child("red")); }
void Color::set_red(double val) { auto c = n_.find_child("red"); if (c.invalid()) { c = n_.append_child() << ryml::key("red"); } detail::SetPrimitive(c, val); }
double Color::green() { return detail::NodeAsPrimitive<double>(n_.find_child("green")); }
void Color::set_green(double val) { auto c = n_.find_child("green"); if (c.invalid()) { c = n_.append_child() << ryml::key("green"); } detail::SetPrimitive(c, val); }
double Color::blue() { return detail::NodeAsPrimitive<double>(n_.find_child("blue")); }
void Color::set_blue(double val) { auto c = n_.find_child("blue"); if (c.invalid()) { c = n_.append_child() << ryml::key("blue"); } detail::SetPrimitive(c, val); }
double Color::alpha() { return detail::NodeAsPrimitive<double>(n_.find_child("alpha")); }
void Color::set_alpha(double val) { auto c = n_.find_child("alpha"); if (c.invalid()) { c = n_.append_child() << ryml::key("alpha"); } detail::SetPrimitive(c, val); }
void Color::init() { StructWrapper::init();  }
bool DocumentColorOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool DocumentColorOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void DocumentColorOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void DocumentColorOptions::init() { StructWrapper::init();  }
bool FoldingRangeOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool FoldingRangeOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void FoldingRangeOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void FoldingRangeOptions::init() { StructWrapper::init();  }
bool DeclarationOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool DeclarationOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void DeclarationOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void DeclarationOptions::init() { StructWrapper::init();  }
std::uint32_t Position::line() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("line")); }
void Position::set_line(std::uint32_t val) { auto c = n_.find_child("line"); if (c.invalid()) { c = n_.append_child() << ryml::key("line"); } detail::SetPrimitive(c, val); }
std::uint32_t Position::character() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("character")); }
void Position::set_character(std::uint32_t val) { auto c = n_.find_child("character"); if (c.invalid()) { c = n_.append_child() << ryml::key("character"); } detail::SetPrimitive(c, val); }
void Position::init() { StructWrapper::init();  }
bool SelectionRangeOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool SelectionRangeOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void SelectionRangeOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void SelectionRangeOptions::init() { StructWrapper::init();  }
bool CallHierarchyOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool CallHierarchyOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void CallHierarchyOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void CallHierarchyOptions::init() { StructWrapper::init();  }
SemanticTokensLegend SemanticTokensOptions::legend() { return n_.find_child("legend"); }
bool SemanticTokensOptions::Union1::holds_boolean() const { return n_.has_child("TODO"); }
bool SemanticTokensOptions::Union1::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void SemanticTokensOptions::Union1::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
SemanticTokensOptions::Union1 SemanticTokensOptions::range() { return n_.find_child("range"); }
bool SemanticTokensOptions::has_range() const { return n_.has_child("range"); }
SemanticTokensOptions::Union1 SemanticTokensOptions::add_range() { auto w = SemanticTokensOptions::Union1(n_.append_child() << ryml::key("range")); w.init(); return w; }
bool SemanticTokensOptions::Union2::holds_boolean() const { return n_.has_child("TODO"); }
bool SemanticTokensOptions::Union2::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void SemanticTokensOptions::Union2::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool SemanticTokensOptions::Union2::holds_SemanticTokensFullDelta() const { return n_.has_child("TODO"); }
SemanticTokensFullDelta SemanticTokensOptions::Union2::as_SemanticTokensFullDelta() { return {n_}; }
SemanticTokensOptions::Union2 SemanticTokensOptions::full() { return n_.find_child("full"); }
bool SemanticTokensOptions::has_full() const { return n_.has_child("full"); }
SemanticTokensOptions::Union2 SemanticTokensOptions::add_full() { auto w = SemanticTokensOptions::Union2(n_.append_child() << ryml::key("full")); w.init(); return w; }
bool SemanticTokensOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool SemanticTokensOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void SemanticTokensOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void SemanticTokensOptions::init() { StructWrapper::init(); SemanticTokensLegend(n_.append_child() << ryml::key("legend")).init(); }
std::uint32_t SemanticTokensEdit::start() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("start")); }
void SemanticTokensEdit::set_start(std::uint32_t val) { auto c = n_.find_child("start"); if (c.invalid()) { c = n_.append_child() << ryml::key("start"); } detail::SetPrimitive(c, val); }
std::uint32_t SemanticTokensEdit::deleteCount() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("deleteCount")); }
void SemanticTokensEdit::set_deleteCount(std::uint32_t val) { auto c = n_.find_child("deleteCount"); if (c.invalid()) { c = n_.append_child() << ryml::key("deleteCount"); } detail::SetPrimitive(c, val); }
List<std::uint32_t> SemanticTokensEdit::data() { return n_.find_child("data"); }
bool SemanticTokensEdit::has_data() const { return n_.has_child("data"); }
List<std::uint32_t> SemanticTokensEdit::add_data() { auto w = List<std::uint32_t>(n_.append_child() << ryml::key("data")); w.init(); return w; }
void SemanticTokensEdit::init() { StructWrapper::init();  }
bool LinkedEditingRangeOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool LinkedEditingRangeOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void LinkedEditingRangeOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void LinkedEditingRangeOptions::init() { StructWrapper::init();  }
std::string_view FileCreate::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void FileCreate::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
void FileCreate::init() { StructWrapper::init();  }
OptionalVersionedTextDocumentIdentifier TextDocumentEdit::textDocument() { return n_.find_child("textDocument"); }
List<std::variant<TextEdit, AnnotatedTextEdit, SnippetTextEdit>> TextDocumentEdit::edits() { return n_.find_child("edits"); }
void TextDocumentEdit::init() { StructWrapper::init(); OptionalVersionedTextDocumentIdentifier(n_.append_child() << ryml::key("textDocument")).init(); List<std::variant<TextEdit, AnnotatedTextEdit, SnippetTextEdit>>(n_.append_child() << ryml::key("edits")).init(); }
std::string_view CreateFile::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void CreateFile::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
CreateFileOptions CreateFile::options() { return n_.find_child("options"); }
bool CreateFile::has_options() const { return n_.has_child("options"); }
CreateFileOptions CreateFile::add_options() { auto w = CreateFileOptions(n_.append_child() << ryml::key("options")); w.init(); return w; }
ChangeAnnotationIdentifier CreateFile::annotationId() { return detail::NodeAsPrimitive<ChangeAnnotationIdentifier>(n_.find_child("annotationId")); }
bool CreateFile::has_annotationId() const { return n_.has_child("annotationId"); }
void CreateFile::set_annotationId(ChangeAnnotationIdentifier val) { auto c = n_.find_child("annotationId"); if (c.invalid()) { c = n_.append_child() << ryml::key("annotationId"); } detail::SetPrimitive(c, val); }
void CreateFile::init() { StructWrapper::init();  }
std::string_view RenameFile::oldUri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("oldUri")); }
void RenameFile::set_oldUri(std::string_view val) { auto c = n_.find_child("oldUri"); if (c.invalid()) { c = n_.append_child() << ryml::key("oldUri"); } detail::SetPrimitive(c, val); }
std::string_view RenameFile::newUri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("newUri")); }
void RenameFile::set_newUri(std::string_view val) { auto c = n_.find_child("newUri"); if (c.invalid()) { c = n_.append_child() << ryml::key("newUri"); } detail::SetPrimitive(c, val); }
RenameFileOptions RenameFile::options() { return n_.find_child("options"); }
bool RenameFile::has_options() const { return n_.has_child("options"); }
RenameFileOptions RenameFile::add_options() { auto w = RenameFileOptions(n_.append_child() << ryml::key("options")); w.init(); return w; }
ChangeAnnotationIdentifier RenameFile::annotationId() { return detail::NodeAsPrimitive<ChangeAnnotationIdentifier>(n_.find_child("annotationId")); }
bool RenameFile::has_annotationId() const { return n_.has_child("annotationId"); }
void RenameFile::set_annotationId(ChangeAnnotationIdentifier val) { auto c = n_.find_child("annotationId"); if (c.invalid()) { c = n_.append_child() << ryml::key("annotationId"); } detail::SetPrimitive(c, val); }
void RenameFile::init() { StructWrapper::init();  }
std::string_view DeleteFile::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void DeleteFile::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
DeleteFileOptions DeleteFile::options() { return n_.find_child("options"); }
bool DeleteFile::has_options() const { return n_.has_child("options"); }
DeleteFileOptions DeleteFile::add_options() { auto w = DeleteFileOptions(n_.append_child() << ryml::key("options")); w.init(); return w; }
ChangeAnnotationIdentifier DeleteFile::annotationId() { return detail::NodeAsPrimitive<ChangeAnnotationIdentifier>(n_.find_child("annotationId")); }
bool DeleteFile::has_annotationId() const { return n_.has_child("annotationId"); }
void DeleteFile::set_annotationId(ChangeAnnotationIdentifier val) { auto c = n_.find_child("annotationId"); if (c.invalid()) { c = n_.append_child() << ryml::key("annotationId"); } detail::SetPrimitive(c, val); }
void DeleteFile::init() { StructWrapper::init();  }
std::string_view ChangeAnnotation::label() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("label")); }
void ChangeAnnotation::set_label(std::string_view val) { auto c = n_.find_child("label"); if (c.invalid()) { c = n_.append_child() << ryml::key("label"); } detail::SetPrimitive(c, val); }
bool ChangeAnnotation::needsConfirmation() { return detail::NodeAsPrimitive<bool>(n_.find_child("needsConfirmation")); }
bool ChangeAnnotation::has_needsConfirmation() const { return n_.has_child("needsConfirmation"); }
void ChangeAnnotation::set_needsConfirmation(bool val) { auto c = n_.find_child("needsConfirmation"); if (c.invalid()) { c = n_.append_child() << ryml::key("needsConfirmation"); } detail::SetPrimitive(c, val); }
std::string_view ChangeAnnotation::description() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("description")); }
bool ChangeAnnotation::has_description() const { return n_.has_child("description"); }
void ChangeAnnotation::set_description(std::string_view val) { auto c = n_.find_child("description"); if (c.invalid()) { c = n_.append_child() << ryml::key("description"); } detail::SetPrimitive(c, val); }
void ChangeAnnotation::init() { StructWrapper::init();  }
std::string_view FileOperationFilter::scheme() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("scheme")); }
bool FileOperationFilter::has_scheme() const { return n_.has_child("scheme"); }
void FileOperationFilter::set_scheme(std::string_view val) { auto c = n_.find_child("scheme"); if (c.invalid()) { c = n_.append_child() << ryml::key("scheme"); } detail::SetPrimitive(c, val); }
FileOperationPattern FileOperationFilter::pattern() { return n_.find_child("pattern"); }
void FileOperationFilter::init() { StructWrapper::init(); FileOperationPattern(n_.append_child() << ryml::key("pattern")).init(); }
std::string_view FileRename::oldUri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("oldUri")); }
void FileRename::set_oldUri(std::string_view val) { auto c = n_.find_child("oldUri"); if (c.invalid()) { c = n_.append_child() << ryml::key("oldUri"); } detail::SetPrimitive(c, val); }
std::string_view FileRename::newUri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("newUri")); }
void FileRename::set_newUri(std::string_view val) { auto c = n_.find_child("newUri"); if (c.invalid()) { c = n_.append_child() << ryml::key("newUri"); } detail::SetPrimitive(c, val); }
void FileRename::init() { StructWrapper::init();  }
std::string_view FileDelete::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void FileDelete::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
void FileDelete::init() { StructWrapper::init();  }
bool MonikerOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool MonikerOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void MonikerOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void MonikerOptions::init() { StructWrapper::init();  }
bool TypeHierarchyOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool TypeHierarchyOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void TypeHierarchyOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void TypeHierarchyOptions::init() { StructWrapper::init();  }
std::int32_t InlineValueContext::frameId() { return detail::NodeAsPrimitive<std::int32_t>(n_.find_child("frameId")); }
void InlineValueContext::set_frameId(std::int32_t val) { auto c = n_.find_child("frameId"); if (c.invalid()) { c = n_.append_child() << ryml::key("frameId"); } detail::SetPrimitive(c, val); }
Range InlineValueContext::stoppedLocation() { return n_.find_child("stoppedLocation"); }
void InlineValueContext::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("stoppedLocation")).init(); }
Range InlineValueText::range() { return n_.find_child("range"); }
std::string_view InlineValueText::text() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("text")); }
void InlineValueText::set_text(std::string_view val) { auto c = n_.find_child("text"); if (c.invalid()) { c = n_.append_child() << ryml::key("text"); } detail::SetPrimitive(c, val); }
void InlineValueText::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); }
Range InlineValueVariableLookup::range() { return n_.find_child("range"); }
std::string_view InlineValueVariableLookup::variableName() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("variableName")); }
bool InlineValueVariableLookup::has_variableName() const { return n_.has_child("variableName"); }
void InlineValueVariableLookup::set_variableName(std::string_view val) { auto c = n_.find_child("variableName"); if (c.invalid()) { c = n_.append_child() << ryml::key("variableName"); } detail::SetPrimitive(c, val); }
bool InlineValueVariableLookup::caseSensitiveLookup() { return detail::NodeAsPrimitive<bool>(n_.find_child("caseSensitiveLookup")); }
void InlineValueVariableLookup::set_caseSensitiveLookup(bool val) { auto c = n_.find_child("caseSensitiveLookup"); if (c.invalid()) { c = n_.append_child() << ryml::key("caseSensitiveLookup"); } detail::SetPrimitive(c, val); }
void InlineValueVariableLookup::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); }
Range InlineValueEvaluatableExpression::range() { return n_.find_child("range"); }
std::string_view InlineValueEvaluatableExpression::expression() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("expression")); }
bool InlineValueEvaluatableExpression::has_expression() const { return n_.has_child("expression"); }
void InlineValueEvaluatableExpression::set_expression(std::string_view val) { auto c = n_.find_child("expression"); if (c.invalid()) { c = n_.append_child() << ryml::key("expression"); } detail::SetPrimitive(c, val); }
void InlineValueEvaluatableExpression::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); }
bool InlineValueOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool InlineValueOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void InlineValueOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void InlineValueOptions::init() { StructWrapper::init();  }
std::string_view InlayHintLabelPart::value() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("value")); }
void InlayHintLabelPart::set_value(std::string_view val) { auto c = n_.find_child("value"); if (c.invalid()) { c = n_.append_child() << ryml::key("value"); } detail::SetPrimitive(c, val); }
bool InlayHintLabelPart::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view InlayHintLabelPart::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void InlayHintLabelPart::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool InlayHintLabelPart::Union1::holds_MarkupContent() const { return n_.has_child("TODO"); }
MarkupContent InlayHintLabelPart::Union1::as_MarkupContent() { return {n_}; }
InlayHintLabelPart::Union1 InlayHintLabelPart::tooltip() { return n_.find_child("tooltip"); }
bool InlayHintLabelPart::has_tooltip() const { return n_.has_child("tooltip"); }
InlayHintLabelPart::Union1 InlayHintLabelPart::add_tooltip() { auto w = InlayHintLabelPart::Union1(n_.append_child() << ryml::key("tooltip")); w.init(); return w; }
Location InlayHintLabelPart::location() { return n_.find_child("location"); }
bool InlayHintLabelPart::has_location() const { return n_.has_child("location"); }
Location InlayHintLabelPart::add_location() { auto w = Location(n_.append_child() << ryml::key("location")); w.init(); return w; }
Command InlayHintLabelPart::command() { return n_.find_child("command"); }
bool InlayHintLabelPart::has_command() const { return n_.has_child("command"); }
Command InlayHintLabelPart::add_command() { auto w = Command(n_.append_child() << ryml::key("command")); w.init(); return w; }
void InlayHintLabelPart::init() { StructWrapper::init();  }
MarkupKind MarkupContent::kind() { return detail::DeserializeEnum<MarkupKind>(n_.find_child("kind")); }
void MarkupContent::set_kind(MarkupKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
std::string_view MarkupContent::value() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("value")); }
void MarkupContent::set_value(std::string_view val) { auto c = n_.find_child("value"); if (c.invalid()) { c = n_.append_child() << ryml::key("value"); } detail::SetPrimitive(c, val); }
void MarkupContent::init() { StructWrapper::init();  }
bool InlayHintOptions::resolveProvider() { return detail::NodeAsPrimitive<bool>(n_.find_child("resolveProvider")); }
bool InlayHintOptions::has_resolveProvider() const { return n_.has_child("resolveProvider"); }
void InlayHintOptions::set_resolveProvider(bool val) { auto c = n_.find_child("resolveProvider"); if (c.invalid()) { c = n_.append_child() << ryml::key("resolveProvider"); } detail::SetPrimitive(c, val); }
bool InlayHintOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool InlayHintOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void InlayHintOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void InlayHintOptions::init() { StructWrapper::init();  }
Dict<std::string_view, std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>> RelatedFullDocumentDiagnosticReport::relatedDocuments() { return n_.find_child("relatedDocuments"); }
bool RelatedFullDocumentDiagnosticReport::has_relatedDocuments() const { return n_.has_child("relatedDocuments"); }
Dict<std::string_view, std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>> RelatedFullDocumentDiagnosticReport::add_relatedDocuments() { auto w = Dict<std::string_view, std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>>(n_.append_child() << ryml::key("relatedDocuments")); w.init(); return w; }
std::string_view RelatedFullDocumentDiagnosticReport::resultId() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("resultId")); }
bool RelatedFullDocumentDiagnosticReport::has_resultId() const { return n_.has_child("resultId"); }
void RelatedFullDocumentDiagnosticReport::set_resultId(std::string_view val) { auto c = n_.find_child("resultId"); if (c.invalid()) { c = n_.append_child() << ryml::key("resultId"); } detail::SetPrimitive(c, val); }
List<Diagnostic> RelatedFullDocumentDiagnosticReport::items() { return n_.find_child("items"); }
void RelatedFullDocumentDiagnosticReport::init() { StructWrapper::init(); List<Diagnostic>(n_.append_child() << ryml::key("items")).init(); }
Dict<std::string_view, std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>> RelatedUnchangedDocumentDiagnosticReport::relatedDocuments() { return n_.find_child("relatedDocuments"); }
bool RelatedUnchangedDocumentDiagnosticReport::has_relatedDocuments() const { return n_.has_child("relatedDocuments"); }
Dict<std::string_view, std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>> RelatedUnchangedDocumentDiagnosticReport::add_relatedDocuments() { auto w = Dict<std::string_view, std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>>(n_.append_child() << ryml::key("relatedDocuments")); w.init(); return w; }
std::string_view RelatedUnchangedDocumentDiagnosticReport::resultId() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("resultId")); }
void RelatedUnchangedDocumentDiagnosticReport::set_resultId(std::string_view val) { auto c = n_.find_child("resultId"); if (c.invalid()) { c = n_.append_child() << ryml::key("resultId"); } detail::SetPrimitive(c, val); }
void RelatedUnchangedDocumentDiagnosticReport::init() { StructWrapper::init();  }
std::string_view FullDocumentDiagnosticReport::resultId() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("resultId")); }
bool FullDocumentDiagnosticReport::has_resultId() const { return n_.has_child("resultId"); }
void FullDocumentDiagnosticReport::set_resultId(std::string_view val) { auto c = n_.find_child("resultId"); if (c.invalid()) { c = n_.append_child() << ryml::key("resultId"); } detail::SetPrimitive(c, val); }
List<Diagnostic> FullDocumentDiagnosticReport::items() { return n_.find_child("items"); }
void FullDocumentDiagnosticReport::init() { StructWrapper::init(); List<Diagnostic>(n_.append_child() << ryml::key("items")).init(); }
std::string_view UnchangedDocumentDiagnosticReport::resultId() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("resultId")); }
void UnchangedDocumentDiagnosticReport::set_resultId(std::string_view val) { auto c = n_.find_child("resultId"); if (c.invalid()) { c = n_.append_child() << ryml::key("resultId"); } detail::SetPrimitive(c, val); }
void UnchangedDocumentDiagnosticReport::init() { StructWrapper::init();  }
std::string_view DiagnosticOptions::identifier() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("identifier")); }
bool DiagnosticOptions::has_identifier() const { return n_.has_child("identifier"); }
void DiagnosticOptions::set_identifier(std::string_view val) { auto c = n_.find_child("identifier"); if (c.invalid()) { c = n_.append_child() << ryml::key("identifier"); } detail::SetPrimitive(c, val); }
bool DiagnosticOptions::interFileDependencies() { return detail::NodeAsPrimitive<bool>(n_.find_child("interFileDependencies")); }
void DiagnosticOptions::set_interFileDependencies(bool val) { auto c = n_.find_child("interFileDependencies"); if (c.invalid()) { c = n_.append_child() << ryml::key("interFileDependencies"); } detail::SetPrimitive(c, val); }
bool DiagnosticOptions::workspaceDiagnostics() { return detail::NodeAsPrimitive<bool>(n_.find_child("workspaceDiagnostics")); }
void DiagnosticOptions::set_workspaceDiagnostics(bool val) { auto c = n_.find_child("workspaceDiagnostics"); if (c.invalid()) { c = n_.append_child() << ryml::key("workspaceDiagnostics"); } detail::SetPrimitive(c, val); }
bool DiagnosticOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool DiagnosticOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void DiagnosticOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void DiagnosticOptions::init() { StructWrapper::init();  }
std::string_view PreviousResultId::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void PreviousResultId::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
std::string_view PreviousResultId::value() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("value")); }
void PreviousResultId::set_value(std::string_view val) { auto c = n_.find_child("value"); if (c.invalid()) { c = n_.append_child() << ryml::key("value"); } detail::SetPrimitive(c, val); }
void PreviousResultId::init() { StructWrapper::init();  }
std::string_view NotebookDocument::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void NotebookDocument::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
std::string_view NotebookDocument::notebookType() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("notebookType")); }
void NotebookDocument::set_notebookType(std::string_view val) { auto c = n_.find_child("notebookType"); if (c.invalid()) { c = n_.append_child() << ryml::key("notebookType"); } detail::SetPrimitive(c, val); }
std::int32_t NotebookDocument::version() { return detail::NodeAsPrimitive<std::int32_t>(n_.find_child("version")); }
void NotebookDocument::set_version(std::int32_t val) { auto c = n_.find_child("version"); if (c.invalid()) { c = n_.append_child() << ryml::key("version"); } detail::SetPrimitive(c, val); }
LSPObject NotebookDocument::metadata() { return n_.find_child("metadata"); }
bool NotebookDocument::has_metadata() const { return n_.has_child("metadata"); }
LSPObject NotebookDocument::add_metadata() { auto w = LSPObject(n_.append_child() << ryml::key("metadata")); w.init(); return w; }
List<NotebookCell> NotebookDocument::cells() { return n_.find_child("cells"); }
void NotebookDocument::init() { StructWrapper::init(); List<NotebookCell>(n_.append_child() << ryml::key("cells")).init(); }
std::string_view TextDocumentItem::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void TextDocumentItem::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
LanguageKind TextDocumentItem::languageId() { return detail::DeserializeEnum<LanguageKind>(n_.find_child("languageId")); }
void TextDocumentItem::set_languageId(LanguageKind val) { auto c = n_.find_child("languageId"); if (c.invalid()) { c = n_.append_child() << ryml::key("languageId"); } detail::SerializeEnum(val, c); }
std::int32_t TextDocumentItem::version() { return detail::NodeAsPrimitive<std::int32_t>(n_.find_child("version")); }
void TextDocumentItem::set_version(std::int32_t val) { auto c = n_.find_child("version"); if (c.invalid()) { c = n_.append_child() << ryml::key("version"); } detail::SetPrimitive(c, val); }
std::string_view TextDocumentItem::text() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("text")); }
void TextDocumentItem::set_text(std::string_view val) { auto c = n_.find_child("text"); if (c.invalid()) { c = n_.append_child() << ryml::key("text"); } detail::SetPrimitive(c, val); }
void TextDocumentItem::init() { StructWrapper::init();  }
List<std::variant<NotebookDocumentFilterWithNotebook, NotebookDocumentFilterWithCells>> NotebookDocumentSyncOptions::notebookSelector() { return n_.find_child("notebookSelector"); }
bool NotebookDocumentSyncOptions::save() { return detail::NodeAsPrimitive<bool>(n_.find_child("save")); }
bool NotebookDocumentSyncOptions::has_save() const { return n_.has_child("save"); }
void NotebookDocumentSyncOptions::set_save(bool val) { auto c = n_.find_child("save"); if (c.invalid()) { c = n_.append_child() << ryml::key("save"); } detail::SetPrimitive(c, val); }
void NotebookDocumentSyncOptions::init() { StructWrapper::init(); List<std::variant<NotebookDocumentFilterWithNotebook, NotebookDocumentFilterWithCells>>(n_.append_child() << ryml::key("notebookSelector")).init(); }
std::int32_t VersionedNotebookDocumentIdentifier::version() { return detail::NodeAsPrimitive<std::int32_t>(n_.find_child("version")); }
void VersionedNotebookDocumentIdentifier::set_version(std::int32_t val) { auto c = n_.find_child("version"); if (c.invalid()) { c = n_.append_child() << ryml::key("version"); } detail::SetPrimitive(c, val); }
std::string_view VersionedNotebookDocumentIdentifier::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void VersionedNotebookDocumentIdentifier::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
void VersionedNotebookDocumentIdentifier::init() { StructWrapper::init();  }
LSPObject NotebookDocumentChangeEvent::metadata() { return n_.find_child("metadata"); }
bool NotebookDocumentChangeEvent::has_metadata() const { return n_.has_child("metadata"); }
LSPObject NotebookDocumentChangeEvent::add_metadata() { auto w = LSPObject(n_.append_child() << ryml::key("metadata")); w.init(); return w; }
NotebookDocumentCellChanges NotebookDocumentChangeEvent::cells() { return n_.find_child("cells"); }
bool NotebookDocumentChangeEvent::has_cells() const { return n_.has_child("cells"); }
NotebookDocumentCellChanges NotebookDocumentChangeEvent::add_cells() { auto w = NotebookDocumentCellChanges(n_.append_child() << ryml::key("cells")); w.init(); return w; }
void NotebookDocumentChangeEvent::init() { StructWrapper::init();  }
std::string_view NotebookDocumentIdentifier::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void NotebookDocumentIdentifier::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
void NotebookDocumentIdentifier::init() { StructWrapper::init();  }
InlineCompletionTriggerKind InlineCompletionContext::triggerKind() { return detail::DeserializeEnum<InlineCompletionTriggerKind>(n_.find_child("triggerKind")); }
void InlineCompletionContext::set_triggerKind(InlineCompletionTriggerKind val) { auto c = n_.find_child("triggerKind"); if (c.invalid()) { c = n_.append_child() << ryml::key("triggerKind"); } detail::SerializeEnum(val, c); }
SelectedCompletionInfo InlineCompletionContext::selectedCompletionInfo() { return n_.find_child("selectedCompletionInfo"); }
bool InlineCompletionContext::has_selectedCompletionInfo() const { return n_.has_child("selectedCompletionInfo"); }
SelectedCompletionInfo InlineCompletionContext::add_selectedCompletionInfo() { auto w = SelectedCompletionInfo(n_.append_child() << ryml::key("selectedCompletionInfo")); w.init(); return w; }
void InlineCompletionContext::init() { StructWrapper::init();  }
std::string_view StringValue::value() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("value")); }
void StringValue::set_value(std::string_view val) { auto c = n_.find_child("value"); if (c.invalid()) { c = n_.append_child() << ryml::key("value"); } detail::SetPrimitive(c, val); }
void StringValue::init() { StructWrapper::init();  }
bool InlineCompletionOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool InlineCompletionOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void InlineCompletionOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void InlineCompletionOptions::init() { StructWrapper::init();  }
List<std::string_view> TextDocumentContentOptions::schemes() { return n_.find_child("schemes"); }
void TextDocumentContentOptions::init() { StructWrapper::init(); List<std::string_view>(n_.append_child() << ryml::key("schemes")).init(); }
std::string_view Registration::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
void Registration::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
std::string_view Registration::method() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("method")); }
void Registration::set_method(std::string_view val) { auto c = n_.find_child("method"); if (c.invalid()) { c = n_.append_child() << ryml::key("method"); } detail::SetPrimitive(c, val); }
bool Registration::Union2::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject Registration::Union2::as_reference_Map0() {  }
bool Registration::Union2::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray Registration::Union2::as_reference_LSPAny() { return {n_}; }
bool Registration::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view Registration::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void Registration::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool Registration::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t Registration::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void Registration::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool Registration::Union2::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t Registration::Union2::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void Registration::Union2::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool Registration::Union2::holds_decimal() const { return n_.has_child("TODO"); }
double Registration::Union2::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void Registration::Union2::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool Registration::Union2::holds_boolean() const { return n_.has_child("TODO"); }
bool Registration::Union2::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void Registration::Union2::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
Registration::Union2 Registration::registerOptions() { return n_.find_child("registerOptions"); }
bool Registration::has_registerOptions() const { return n_.has_child("registerOptions"); }
Registration::Union2 Registration::add_registerOptions() { auto w = Registration::Union2(n_.append_child() << ryml::key("registerOptions")); w.init(); return w; }
void Registration::init() { StructWrapper::init();  }
std::string_view Unregistration::id() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("id")); }
void Unregistration::set_id(std::string_view val) { auto c = n_.find_child("id"); if (c.invalid()) { c = n_.append_child() << ryml::key("id"); } detail::SetPrimitive(c, val); }
std::string_view Unregistration::method() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("method")); }
void Unregistration::set_method(std::string_view val) { auto c = n_.find_child("method"); if (c.invalid()) { c = n_.append_child() << ryml::key("method"); } detail::SetPrimitive(c, val); }
void Unregistration::init() { StructWrapper::init();  }
bool _InitializeParams::Union0::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t _InitializeParams::Union0::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void _InitializeParams::Union0::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
_InitializeParams::Union0 _InitializeParams::processId() { return n_.find_child("processId"); }
ClientInfo _InitializeParams::clientInfo() { return n_.find_child("clientInfo"); }
bool _InitializeParams::has_clientInfo() const { return n_.has_child("clientInfo"); }
ClientInfo _InitializeParams::add_clientInfo() { auto w = ClientInfo(n_.append_child() << ryml::key("clientInfo")); w.init(); return w; }
std::string_view _InitializeParams::locale() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("locale")); }
bool _InitializeParams::has_locale() const { return n_.has_child("locale"); }
void _InitializeParams::set_locale(std::string_view val) { auto c = n_.find_child("locale"); if (c.invalid()) { c = n_.append_child() << ryml::key("locale"); } detail::SetPrimitive(c, val); }
bool _InitializeParams::Union3::holds_string() const { return n_.has_child("TODO"); }
std::string_view _InitializeParams::Union3::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void _InitializeParams::Union3::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
_InitializeParams::Union3 _InitializeParams::rootPath() { return n_.find_child("rootPath"); }
bool _InitializeParams::has_rootPath() const { return n_.has_child("rootPath"); }
_InitializeParams::Union3 _InitializeParams::add_rootPath() { auto w = _InitializeParams::Union3(n_.append_child() << ryml::key("rootPath")); w.init(); return w; }
bool _InitializeParams::Union4::holds_DocumentUri() const { return n_.has_child("TODO"); }
std::string_view _InitializeParams::Union4::as_DocumentUri() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void _InitializeParams::Union4::set_DocumentUri(std::string_view val) { detail::SetPrimitive(n_, val); }
_InitializeParams::Union4 _InitializeParams::rootUri() { return n_.find_child("rootUri"); }
ClientCapabilities _InitializeParams::capabilities() { return n_.find_child("capabilities"); }
bool _InitializeParams::Union6::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject _InitializeParams::Union6::as_reference_Map0() {  }
bool _InitializeParams::Union6::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray _InitializeParams::Union6::as_reference_LSPAny() { return {n_}; }
bool _InitializeParams::Union6::holds_string() const { return n_.has_child("TODO"); }
std::string_view _InitializeParams::Union6::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void _InitializeParams::Union6::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool _InitializeParams::Union6::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t _InitializeParams::Union6::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void _InitializeParams::Union6::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool _InitializeParams::Union6::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t _InitializeParams::Union6::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void _InitializeParams::Union6::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool _InitializeParams::Union6::holds_decimal() const { return n_.has_child("TODO"); }
double _InitializeParams::Union6::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void _InitializeParams::Union6::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool _InitializeParams::Union6::holds_boolean() const { return n_.has_child("TODO"); }
bool _InitializeParams::Union6::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void _InitializeParams::Union6::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
_InitializeParams::Union6 _InitializeParams::initializationOptions() { return n_.find_child("initializationOptions"); }
bool _InitializeParams::has_initializationOptions() const { return n_.has_child("initializationOptions"); }
_InitializeParams::Union6 _InitializeParams::add_initializationOptions() { auto w = _InitializeParams::Union6(n_.append_child() << ryml::key("initializationOptions")); w.init(); return w; }
TraceValue _InitializeParams::trace() { return detail::DeserializeEnum<TraceValue>(n_.find_child("trace")); }
bool _InitializeParams::has_trace() const { return n_.has_child("trace"); }
void _InitializeParams::set_trace(TraceValue val) { auto c = n_.find_child("trace"); if (c.invalid()) { c = n_.append_child() << ryml::key("trace"); } detail::SerializeEnum(val, c); }
bool _InitializeParams::Union8::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t _InitializeParams::Union8::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void _InitializeParams::Union8::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool _InitializeParams::Union8::holds_string() const { return n_.has_child("TODO"); }
std::string_view _InitializeParams::Union8::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void _InitializeParams::Union8::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
_InitializeParams::Union8 _InitializeParams::workDoneToken() { return n_.find_child("workDoneToken"); }
bool _InitializeParams::has_workDoneToken() const { return n_.has_child("workDoneToken"); }
_InitializeParams::Union8 _InitializeParams::add_workDoneToken() { auto w = _InitializeParams::Union8(n_.append_child() << ryml::key("workDoneToken")); w.init(); return w; }
void _InitializeParams::init() { StructWrapper::init(); _InitializeParams::Union0(n_.append_child() << ryml::key("processId")).init(); _InitializeParams::Union4(n_.append_child() << ryml::key("rootUri")).init(); ClientCapabilities(n_.append_child() << ryml::key("capabilities")).init(); }
bool WorkspaceFoldersInitializeParams::Union0::holds_array_WorkspaceFolder() const { return n_.has_child("TODO"); }
List<WorkspaceFolder> WorkspaceFoldersInitializeParams::Union0::as_array_WorkspaceFolder() { return {n_}; }
WorkspaceFoldersInitializeParams::Union0 WorkspaceFoldersInitializeParams::workspaceFolders() { return n_.find_child("workspaceFolders"); }
bool WorkspaceFoldersInitializeParams::has_workspaceFolders() const { return n_.has_child("workspaceFolders"); }
WorkspaceFoldersInitializeParams::Union0 WorkspaceFoldersInitializeParams::add_workspaceFolders() { auto w = WorkspaceFoldersInitializeParams::Union0(n_.append_child() << ryml::key("workspaceFolders")); w.init(); return w; }
void WorkspaceFoldersInitializeParams::init() { StructWrapper::init();  }
PositionEncodingKind ServerCapabilities::positionEncoding() { return detail::DeserializeEnum<PositionEncodingKind>(n_.find_child("positionEncoding")); }
bool ServerCapabilities::has_positionEncoding() const { return n_.has_child("positionEncoding"); }
void ServerCapabilities::set_positionEncoding(PositionEncodingKind val) { auto c = n_.find_child("positionEncoding"); if (c.invalid()) { c = n_.append_child() << ryml::key("positionEncoding"); } detail::SerializeEnum(val, c); }
bool ServerCapabilities::Union1::holds_TextDocumentSyncOptions() const { return n_.has_child("TODO"); }
TextDocumentSyncOptions ServerCapabilities::Union1::as_TextDocumentSyncOptions() { return {n_}; }
bool ServerCapabilities::Union1::holds_TextDocumentSyncKind() const { return n_.has_child("TODO"); }
TextDocumentSyncKind ServerCapabilities::Union1::as_TextDocumentSyncKind() { return detail::DeserializeEnum<TextDocumentSyncKind>(n_); }
ServerCapabilities::Union1 ServerCapabilities::textDocumentSync() { return n_.find_child("textDocumentSync"); }
bool ServerCapabilities::has_textDocumentSync() const { return n_.has_child("textDocumentSync"); }
ServerCapabilities::Union1 ServerCapabilities::add_textDocumentSync() { auto w = ServerCapabilities::Union1(n_.append_child() << ryml::key("textDocumentSync")); w.init(); return w; }
bool ServerCapabilities::Union2::holds_NotebookDocumentSyncOptions() const { return n_.has_child("TODO"); }
NotebookDocumentSyncOptions ServerCapabilities::Union2::as_NotebookDocumentSyncOptions() { return {n_}; }
bool ServerCapabilities::Union2::holds_NotebookDocumentSyncRegistrationOptions() const { return n_.has_child("TODO"); }
NotebookDocumentSyncRegistrationOptions ServerCapabilities::Union2::as_NotebookDocumentSyncRegistrationOptions() { return {n_}; }
ServerCapabilities::Union2 ServerCapabilities::notebookDocumentSync() { return n_.find_child("notebookDocumentSync"); }
bool ServerCapabilities::has_notebookDocumentSync() const { return n_.has_child("notebookDocumentSync"); }
ServerCapabilities::Union2 ServerCapabilities::add_notebookDocumentSync() { auto w = ServerCapabilities::Union2(n_.append_child() << ryml::key("notebookDocumentSync")); w.init(); return w; }
CompletionOptions ServerCapabilities::completionProvider() { return n_.find_child("completionProvider"); }
bool ServerCapabilities::has_completionProvider() const { return n_.has_child("completionProvider"); }
CompletionOptions ServerCapabilities::add_completionProvider() { auto w = CompletionOptions(n_.append_child() << ryml::key("completionProvider")); w.init(); return w; }
bool ServerCapabilities::Union4::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union4::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union4::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union4::holds_HoverOptions() const { return n_.has_child("TODO"); }
HoverOptions ServerCapabilities::Union4::as_HoverOptions() { return {n_}; }
ServerCapabilities::Union4 ServerCapabilities::hoverProvider() { return n_.find_child("hoverProvider"); }
bool ServerCapabilities::has_hoverProvider() const { return n_.has_child("hoverProvider"); }
ServerCapabilities::Union4 ServerCapabilities::add_hoverProvider() { auto w = ServerCapabilities::Union4(n_.append_child() << ryml::key("hoverProvider")); w.init(); return w; }
SignatureHelpOptions ServerCapabilities::signatureHelpProvider() { return n_.find_child("signatureHelpProvider"); }
bool ServerCapabilities::has_signatureHelpProvider() const { return n_.has_child("signatureHelpProvider"); }
SignatureHelpOptions ServerCapabilities::add_signatureHelpProvider() { auto w = SignatureHelpOptions(n_.append_child() << ryml::key("signatureHelpProvider")); w.init(); return w; }
bool ServerCapabilities::Union6::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union6::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union6::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union6::holds_DeclarationOptions() const { return n_.has_child("TODO"); }
DeclarationOptions ServerCapabilities::Union6::as_DeclarationOptions() { return {n_}; }
bool ServerCapabilities::Union6::holds_DeclarationRegistrationOptions() const { return n_.has_child("TODO"); }
DeclarationRegistrationOptions ServerCapabilities::Union6::as_DeclarationRegistrationOptions() { return {n_}; }
ServerCapabilities::Union6 ServerCapabilities::declarationProvider() { return n_.find_child("declarationProvider"); }
bool ServerCapabilities::has_declarationProvider() const { return n_.has_child("declarationProvider"); }
ServerCapabilities::Union6 ServerCapabilities::add_declarationProvider() { auto w = ServerCapabilities::Union6(n_.append_child() << ryml::key("declarationProvider")); w.init(); return w; }
bool ServerCapabilities::Union7::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union7::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union7::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union7::holds_DefinitionOptions() const { return n_.has_child("TODO"); }
DefinitionOptions ServerCapabilities::Union7::as_DefinitionOptions() { return {n_}; }
ServerCapabilities::Union7 ServerCapabilities::definitionProvider() { return n_.find_child("definitionProvider"); }
bool ServerCapabilities::has_definitionProvider() const { return n_.has_child("definitionProvider"); }
ServerCapabilities::Union7 ServerCapabilities::add_definitionProvider() { auto w = ServerCapabilities::Union7(n_.append_child() << ryml::key("definitionProvider")); w.init(); return w; }
bool ServerCapabilities::Union8::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union8::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union8::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union8::holds_TypeDefinitionOptions() const { return n_.has_child("TODO"); }
TypeDefinitionOptions ServerCapabilities::Union8::as_TypeDefinitionOptions() { return {n_}; }
bool ServerCapabilities::Union8::holds_TypeDefinitionRegistrationOptions() const { return n_.has_child("TODO"); }
TypeDefinitionRegistrationOptions ServerCapabilities::Union8::as_TypeDefinitionRegistrationOptions() { return {n_}; }
ServerCapabilities::Union8 ServerCapabilities::typeDefinitionProvider() { return n_.find_child("typeDefinitionProvider"); }
bool ServerCapabilities::has_typeDefinitionProvider() const { return n_.has_child("typeDefinitionProvider"); }
ServerCapabilities::Union8 ServerCapabilities::add_typeDefinitionProvider() { auto w = ServerCapabilities::Union8(n_.append_child() << ryml::key("typeDefinitionProvider")); w.init(); return w; }
bool ServerCapabilities::Union9::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union9::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union9::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union9::holds_ImplementationOptions() const { return n_.has_child("TODO"); }
ImplementationOptions ServerCapabilities::Union9::as_ImplementationOptions() { return {n_}; }
bool ServerCapabilities::Union9::holds_ImplementationRegistrationOptions() const { return n_.has_child("TODO"); }
ImplementationRegistrationOptions ServerCapabilities::Union9::as_ImplementationRegistrationOptions() { return {n_}; }
ServerCapabilities::Union9 ServerCapabilities::implementationProvider() { return n_.find_child("implementationProvider"); }
bool ServerCapabilities::has_implementationProvider() const { return n_.has_child("implementationProvider"); }
ServerCapabilities::Union9 ServerCapabilities::add_implementationProvider() { auto w = ServerCapabilities::Union9(n_.append_child() << ryml::key("implementationProvider")); w.init(); return w; }
bool ServerCapabilities::Union10::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union10::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union10::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union10::holds_ReferenceOptions() const { return n_.has_child("TODO"); }
ReferenceOptions ServerCapabilities::Union10::as_ReferenceOptions() { return {n_}; }
ServerCapabilities::Union10 ServerCapabilities::referencesProvider() { return n_.find_child("referencesProvider"); }
bool ServerCapabilities::has_referencesProvider() const { return n_.has_child("referencesProvider"); }
ServerCapabilities::Union10 ServerCapabilities::add_referencesProvider() { auto w = ServerCapabilities::Union10(n_.append_child() << ryml::key("referencesProvider")); w.init(); return w; }
bool ServerCapabilities::Union11::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union11::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union11::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union11::holds_DocumentHighlightOptions() const { return n_.has_child("TODO"); }
DocumentHighlightOptions ServerCapabilities::Union11::as_DocumentHighlightOptions() { return {n_}; }
ServerCapabilities::Union11 ServerCapabilities::documentHighlightProvider() { return n_.find_child("documentHighlightProvider"); }
bool ServerCapabilities::has_documentHighlightProvider() const { return n_.has_child("documentHighlightProvider"); }
ServerCapabilities::Union11 ServerCapabilities::add_documentHighlightProvider() { auto w = ServerCapabilities::Union11(n_.append_child() << ryml::key("documentHighlightProvider")); w.init(); return w; }
bool ServerCapabilities::Union12::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union12::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union12::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union12::holds_DocumentSymbolOptions() const { return n_.has_child("TODO"); }
DocumentSymbolOptions ServerCapabilities::Union12::as_DocumentSymbolOptions() { return {n_}; }
ServerCapabilities::Union12 ServerCapabilities::documentSymbolProvider() { return n_.find_child("documentSymbolProvider"); }
bool ServerCapabilities::has_documentSymbolProvider() const { return n_.has_child("documentSymbolProvider"); }
ServerCapabilities::Union12 ServerCapabilities::add_documentSymbolProvider() { auto w = ServerCapabilities::Union12(n_.append_child() << ryml::key("documentSymbolProvider")); w.init(); return w; }
bool ServerCapabilities::Union13::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union13::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union13::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union13::holds_CodeActionOptions() const { return n_.has_child("TODO"); }
CodeActionOptions ServerCapabilities::Union13::as_CodeActionOptions() { return {n_}; }
ServerCapabilities::Union13 ServerCapabilities::codeActionProvider() { return n_.find_child("codeActionProvider"); }
bool ServerCapabilities::has_codeActionProvider() const { return n_.has_child("codeActionProvider"); }
ServerCapabilities::Union13 ServerCapabilities::add_codeActionProvider() { auto w = ServerCapabilities::Union13(n_.append_child() << ryml::key("codeActionProvider")); w.init(); return w; }
CodeLensOptions ServerCapabilities::codeLensProvider() { return n_.find_child("codeLensProvider"); }
bool ServerCapabilities::has_codeLensProvider() const { return n_.has_child("codeLensProvider"); }
CodeLensOptions ServerCapabilities::add_codeLensProvider() { auto w = CodeLensOptions(n_.append_child() << ryml::key("codeLensProvider")); w.init(); return w; }
DocumentLinkOptions ServerCapabilities::documentLinkProvider() { return n_.find_child("documentLinkProvider"); }
bool ServerCapabilities::has_documentLinkProvider() const { return n_.has_child("documentLinkProvider"); }
DocumentLinkOptions ServerCapabilities::add_documentLinkProvider() { auto w = DocumentLinkOptions(n_.append_child() << ryml::key("documentLinkProvider")); w.init(); return w; }
bool ServerCapabilities::Union16::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union16::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union16::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union16::holds_DocumentColorOptions() const { return n_.has_child("TODO"); }
DocumentColorOptions ServerCapabilities::Union16::as_DocumentColorOptions() { return {n_}; }
bool ServerCapabilities::Union16::holds_DocumentColorRegistrationOptions() const { return n_.has_child("TODO"); }
DocumentColorRegistrationOptions ServerCapabilities::Union16::as_DocumentColorRegistrationOptions() { return {n_}; }
ServerCapabilities::Union16 ServerCapabilities::colorProvider() { return n_.find_child("colorProvider"); }
bool ServerCapabilities::has_colorProvider() const { return n_.has_child("colorProvider"); }
ServerCapabilities::Union16 ServerCapabilities::add_colorProvider() { auto w = ServerCapabilities::Union16(n_.append_child() << ryml::key("colorProvider")); w.init(); return w; }
bool ServerCapabilities::Union17::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union17::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union17::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union17::holds_WorkspaceSymbolOptions() const { return n_.has_child("TODO"); }
WorkspaceSymbolOptions ServerCapabilities::Union17::as_WorkspaceSymbolOptions() { return {n_}; }
ServerCapabilities::Union17 ServerCapabilities::workspaceSymbolProvider() { return n_.find_child("workspaceSymbolProvider"); }
bool ServerCapabilities::has_workspaceSymbolProvider() const { return n_.has_child("workspaceSymbolProvider"); }
ServerCapabilities::Union17 ServerCapabilities::add_workspaceSymbolProvider() { auto w = ServerCapabilities::Union17(n_.append_child() << ryml::key("workspaceSymbolProvider")); w.init(); return w; }
bool ServerCapabilities::Union18::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union18::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union18::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union18::holds_DocumentFormattingOptions() const { return n_.has_child("TODO"); }
DocumentFormattingOptions ServerCapabilities::Union18::as_DocumentFormattingOptions() { return {n_}; }
ServerCapabilities::Union18 ServerCapabilities::documentFormattingProvider() { return n_.find_child("documentFormattingProvider"); }
bool ServerCapabilities::has_documentFormattingProvider() const { return n_.has_child("documentFormattingProvider"); }
ServerCapabilities::Union18 ServerCapabilities::add_documentFormattingProvider() { auto w = ServerCapabilities::Union18(n_.append_child() << ryml::key("documentFormattingProvider")); w.init(); return w; }
bool ServerCapabilities::Union19::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union19::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union19::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union19::holds_DocumentRangeFormattingOptions() const { return n_.has_child("TODO"); }
DocumentRangeFormattingOptions ServerCapabilities::Union19::as_DocumentRangeFormattingOptions() { return {n_}; }
ServerCapabilities::Union19 ServerCapabilities::documentRangeFormattingProvider() { return n_.find_child("documentRangeFormattingProvider"); }
bool ServerCapabilities::has_documentRangeFormattingProvider() const { return n_.has_child("documentRangeFormattingProvider"); }
ServerCapabilities::Union19 ServerCapabilities::add_documentRangeFormattingProvider() { auto w = ServerCapabilities::Union19(n_.append_child() << ryml::key("documentRangeFormattingProvider")); w.init(); return w; }
DocumentOnTypeFormattingOptions ServerCapabilities::documentOnTypeFormattingProvider() { return n_.find_child("documentOnTypeFormattingProvider"); }
bool ServerCapabilities::has_documentOnTypeFormattingProvider() const { return n_.has_child("documentOnTypeFormattingProvider"); }
DocumentOnTypeFormattingOptions ServerCapabilities::add_documentOnTypeFormattingProvider() { auto w = DocumentOnTypeFormattingOptions(n_.append_child() << ryml::key("documentOnTypeFormattingProvider")); w.init(); return w; }
bool ServerCapabilities::Union21::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union21::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union21::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union21::holds_RenameOptions() const { return n_.has_child("TODO"); }
RenameOptions ServerCapabilities::Union21::as_RenameOptions() { return {n_}; }
ServerCapabilities::Union21 ServerCapabilities::renameProvider() { return n_.find_child("renameProvider"); }
bool ServerCapabilities::has_renameProvider() const { return n_.has_child("renameProvider"); }
ServerCapabilities::Union21 ServerCapabilities::add_renameProvider() { auto w = ServerCapabilities::Union21(n_.append_child() << ryml::key("renameProvider")); w.init(); return w; }
bool ServerCapabilities::Union22::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union22::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union22::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union22::holds_FoldingRangeOptions() const { return n_.has_child("TODO"); }
FoldingRangeOptions ServerCapabilities::Union22::as_FoldingRangeOptions() { return {n_}; }
bool ServerCapabilities::Union22::holds_FoldingRangeRegistrationOptions() const { return n_.has_child("TODO"); }
FoldingRangeRegistrationOptions ServerCapabilities::Union22::as_FoldingRangeRegistrationOptions() { return {n_}; }
ServerCapabilities::Union22 ServerCapabilities::foldingRangeProvider() { return n_.find_child("foldingRangeProvider"); }
bool ServerCapabilities::has_foldingRangeProvider() const { return n_.has_child("foldingRangeProvider"); }
ServerCapabilities::Union22 ServerCapabilities::add_foldingRangeProvider() { auto w = ServerCapabilities::Union22(n_.append_child() << ryml::key("foldingRangeProvider")); w.init(); return w; }
bool ServerCapabilities::Union23::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union23::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union23::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union23::holds_SelectionRangeOptions() const { return n_.has_child("TODO"); }
SelectionRangeOptions ServerCapabilities::Union23::as_SelectionRangeOptions() { return {n_}; }
bool ServerCapabilities::Union23::holds_SelectionRangeRegistrationOptions() const { return n_.has_child("TODO"); }
SelectionRangeRegistrationOptions ServerCapabilities::Union23::as_SelectionRangeRegistrationOptions() { return {n_}; }
ServerCapabilities::Union23 ServerCapabilities::selectionRangeProvider() { return n_.find_child("selectionRangeProvider"); }
bool ServerCapabilities::has_selectionRangeProvider() const { return n_.has_child("selectionRangeProvider"); }
ServerCapabilities::Union23 ServerCapabilities::add_selectionRangeProvider() { auto w = ServerCapabilities::Union23(n_.append_child() << ryml::key("selectionRangeProvider")); w.init(); return w; }
ExecuteCommandOptions ServerCapabilities::executeCommandProvider() { return n_.find_child("executeCommandProvider"); }
bool ServerCapabilities::has_executeCommandProvider() const { return n_.has_child("executeCommandProvider"); }
ExecuteCommandOptions ServerCapabilities::add_executeCommandProvider() { auto w = ExecuteCommandOptions(n_.append_child() << ryml::key("executeCommandProvider")); w.init(); return w; }
bool ServerCapabilities::Union25::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union25::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union25::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union25::holds_CallHierarchyOptions() const { return n_.has_child("TODO"); }
CallHierarchyOptions ServerCapabilities::Union25::as_CallHierarchyOptions() { return {n_}; }
bool ServerCapabilities::Union25::holds_CallHierarchyRegistrationOptions() const { return n_.has_child("TODO"); }
CallHierarchyRegistrationOptions ServerCapabilities::Union25::as_CallHierarchyRegistrationOptions() { return {n_}; }
ServerCapabilities::Union25 ServerCapabilities::callHierarchyProvider() { return n_.find_child("callHierarchyProvider"); }
bool ServerCapabilities::has_callHierarchyProvider() const { return n_.has_child("callHierarchyProvider"); }
ServerCapabilities::Union25 ServerCapabilities::add_callHierarchyProvider() { auto w = ServerCapabilities::Union25(n_.append_child() << ryml::key("callHierarchyProvider")); w.init(); return w; }
bool ServerCapabilities::Union26::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union26::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union26::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union26::holds_LinkedEditingRangeOptions() const { return n_.has_child("TODO"); }
LinkedEditingRangeOptions ServerCapabilities::Union26::as_LinkedEditingRangeOptions() { return {n_}; }
bool ServerCapabilities::Union26::holds_LinkedEditingRangeRegistrationOptions() const { return n_.has_child("TODO"); }
LinkedEditingRangeRegistrationOptions ServerCapabilities::Union26::as_LinkedEditingRangeRegistrationOptions() { return {n_}; }
ServerCapabilities::Union26 ServerCapabilities::linkedEditingRangeProvider() { return n_.find_child("linkedEditingRangeProvider"); }
bool ServerCapabilities::has_linkedEditingRangeProvider() const { return n_.has_child("linkedEditingRangeProvider"); }
ServerCapabilities::Union26 ServerCapabilities::add_linkedEditingRangeProvider() { auto w = ServerCapabilities::Union26(n_.append_child() << ryml::key("linkedEditingRangeProvider")); w.init(); return w; }
bool ServerCapabilities::Union27::holds_SemanticTokensOptions() const { return n_.has_child("TODO"); }
SemanticTokensOptions ServerCapabilities::Union27::as_SemanticTokensOptions() { return {n_}; }
bool ServerCapabilities::Union27::holds_SemanticTokensRegistrationOptions() const { return n_.has_child("TODO"); }
SemanticTokensRegistrationOptions ServerCapabilities::Union27::as_SemanticTokensRegistrationOptions() { return {n_}; }
ServerCapabilities::Union27 ServerCapabilities::semanticTokensProvider() { return n_.find_child("semanticTokensProvider"); }
bool ServerCapabilities::has_semanticTokensProvider() const { return n_.has_child("semanticTokensProvider"); }
ServerCapabilities::Union27 ServerCapabilities::add_semanticTokensProvider() { auto w = ServerCapabilities::Union27(n_.append_child() << ryml::key("semanticTokensProvider")); w.init(); return w; }
bool ServerCapabilities::Union28::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union28::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union28::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union28::holds_MonikerOptions() const { return n_.has_child("TODO"); }
MonikerOptions ServerCapabilities::Union28::as_MonikerOptions() { return {n_}; }
bool ServerCapabilities::Union28::holds_MonikerRegistrationOptions() const { return n_.has_child("TODO"); }
MonikerRegistrationOptions ServerCapabilities::Union28::as_MonikerRegistrationOptions() { return {n_}; }
ServerCapabilities::Union28 ServerCapabilities::monikerProvider() { return n_.find_child("monikerProvider"); }
bool ServerCapabilities::has_monikerProvider() const { return n_.has_child("monikerProvider"); }
ServerCapabilities::Union28 ServerCapabilities::add_monikerProvider() { auto w = ServerCapabilities::Union28(n_.append_child() << ryml::key("monikerProvider")); w.init(); return w; }
bool ServerCapabilities::Union29::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union29::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union29::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union29::holds_TypeHierarchyOptions() const { return n_.has_child("TODO"); }
TypeHierarchyOptions ServerCapabilities::Union29::as_TypeHierarchyOptions() { return {n_}; }
bool ServerCapabilities::Union29::holds_TypeHierarchyRegistrationOptions() const { return n_.has_child("TODO"); }
TypeHierarchyRegistrationOptions ServerCapabilities::Union29::as_TypeHierarchyRegistrationOptions() { return {n_}; }
ServerCapabilities::Union29 ServerCapabilities::typeHierarchyProvider() { return n_.find_child("typeHierarchyProvider"); }
bool ServerCapabilities::has_typeHierarchyProvider() const { return n_.has_child("typeHierarchyProvider"); }
ServerCapabilities::Union29 ServerCapabilities::add_typeHierarchyProvider() { auto w = ServerCapabilities::Union29(n_.append_child() << ryml::key("typeHierarchyProvider")); w.init(); return w; }
bool ServerCapabilities::Union30::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union30::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union30::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union30::holds_InlineValueOptions() const { return n_.has_child("TODO"); }
InlineValueOptions ServerCapabilities::Union30::as_InlineValueOptions() { return {n_}; }
bool ServerCapabilities::Union30::holds_InlineValueRegistrationOptions() const { return n_.has_child("TODO"); }
InlineValueRegistrationOptions ServerCapabilities::Union30::as_InlineValueRegistrationOptions() { return {n_}; }
ServerCapabilities::Union30 ServerCapabilities::inlineValueProvider() { return n_.find_child("inlineValueProvider"); }
bool ServerCapabilities::has_inlineValueProvider() const { return n_.has_child("inlineValueProvider"); }
ServerCapabilities::Union30 ServerCapabilities::add_inlineValueProvider() { auto w = ServerCapabilities::Union30(n_.append_child() << ryml::key("inlineValueProvider")); w.init(); return w; }
bool ServerCapabilities::Union31::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union31::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union31::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union31::holds_InlayHintOptions() const { return n_.has_child("TODO"); }
InlayHintOptions ServerCapabilities::Union31::as_InlayHintOptions() { return {n_}; }
bool ServerCapabilities::Union31::holds_InlayHintRegistrationOptions() const { return n_.has_child("TODO"); }
InlayHintRegistrationOptions ServerCapabilities::Union31::as_InlayHintRegistrationOptions() { return {n_}; }
ServerCapabilities::Union31 ServerCapabilities::inlayHintProvider() { return n_.find_child("inlayHintProvider"); }
bool ServerCapabilities::has_inlayHintProvider() const { return n_.has_child("inlayHintProvider"); }
ServerCapabilities::Union31 ServerCapabilities::add_inlayHintProvider() { auto w = ServerCapabilities::Union31(n_.append_child() << ryml::key("inlayHintProvider")); w.init(); return w; }
bool ServerCapabilities::Union32::holds_DiagnosticOptions() const { return n_.has_child("TODO"); }
DiagnosticOptions ServerCapabilities::Union32::as_DiagnosticOptions() { return {n_}; }
bool ServerCapabilities::Union32::holds_DiagnosticRegistrationOptions() const { return n_.has_child("TODO"); }
DiagnosticRegistrationOptions ServerCapabilities::Union32::as_DiagnosticRegistrationOptions() { return {n_}; }
ServerCapabilities::Union32 ServerCapabilities::diagnosticProvider() { return n_.find_child("diagnosticProvider"); }
bool ServerCapabilities::has_diagnosticProvider() const { return n_.has_child("diagnosticProvider"); }
ServerCapabilities::Union32 ServerCapabilities::add_diagnosticProvider() { auto w = ServerCapabilities::Union32(n_.append_child() << ryml::key("diagnosticProvider")); w.init(); return w; }
bool ServerCapabilities::Union33::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union33::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union33::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union33::holds_InlineCompletionOptions() const { return n_.has_child("TODO"); }
InlineCompletionOptions ServerCapabilities::Union33::as_InlineCompletionOptions() { return {n_}; }
ServerCapabilities::Union33 ServerCapabilities::inlineCompletionProvider() { return n_.find_child("inlineCompletionProvider"); }
bool ServerCapabilities::has_inlineCompletionProvider() const { return n_.has_child("inlineCompletionProvider"); }
ServerCapabilities::Union33 ServerCapabilities::add_inlineCompletionProvider() { auto w = ServerCapabilities::Union33(n_.append_child() << ryml::key("inlineCompletionProvider")); w.init(); return w; }
WorkspaceOptions ServerCapabilities::workspace() { return n_.find_child("workspace"); }
bool ServerCapabilities::has_workspace() const { return n_.has_child("workspace"); }
WorkspaceOptions ServerCapabilities::add_workspace() { auto w = WorkspaceOptions(n_.append_child() << ryml::key("workspace")); w.init(); return w; }
bool ServerCapabilities::Union35::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject ServerCapabilities::Union35::as_reference_Map0() {  }
bool ServerCapabilities::Union35::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray ServerCapabilities::Union35::as_reference_LSPAny() { return {n_}; }
bool ServerCapabilities::Union35::holds_string() const { return n_.has_child("TODO"); }
std::string_view ServerCapabilities::Union35::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void ServerCapabilities::Union35::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union35::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t ServerCapabilities::Union35::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void ServerCapabilities::Union35::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union35::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t ServerCapabilities::Union35::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void ServerCapabilities::Union35::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union35::holds_decimal() const { return n_.has_child("TODO"); }
double ServerCapabilities::Union35::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void ServerCapabilities::Union35::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool ServerCapabilities::Union35::holds_boolean() const { return n_.has_child("TODO"); }
bool ServerCapabilities::Union35::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ServerCapabilities::Union35::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
ServerCapabilities::Union35 ServerCapabilities::experimental() { return n_.find_child("experimental"); }
bool ServerCapabilities::has_experimental() const { return n_.has_child("experimental"); }
ServerCapabilities::Union35 ServerCapabilities::add_experimental() { auto w = ServerCapabilities::Union35(n_.append_child() << ryml::key("experimental")); w.init(); return w; }
void ServerCapabilities::init() { StructWrapper::init();  }
std::string_view ServerInfo::name() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("name")); }
void ServerInfo::set_name(std::string_view val) { auto c = n_.find_child("name"); if (c.invalid()) { c = n_.append_child() << ryml::key("name"); } detail::SetPrimitive(c, val); }
std::string_view ServerInfo::version() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("version")); }
bool ServerInfo::has_version() const { return n_.has_child("version"); }
void ServerInfo::set_version(std::string_view val) { auto c = n_.find_child("version"); if (c.invalid()) { c = n_.append_child() << ryml::key("version"); } detail::SetPrimitive(c, val); }
void ServerInfo::init() { StructWrapper::init();  }
std::int32_t VersionedTextDocumentIdentifier::version() { return detail::NodeAsPrimitive<std::int32_t>(n_.find_child("version")); }
void VersionedTextDocumentIdentifier::set_version(std::int32_t val) { auto c = n_.find_child("version"); if (c.invalid()) { c = n_.append_child() << ryml::key("version"); } detail::SetPrimitive(c, val); }
std::string_view VersionedTextDocumentIdentifier::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void VersionedTextDocumentIdentifier::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
void VersionedTextDocumentIdentifier::init() { StructWrapper::init();  }
bool SaveOptions::includeText() { return detail::NodeAsPrimitive<bool>(n_.find_child("includeText")); }
bool SaveOptions::has_includeText() const { return n_.has_child("includeText"); }
void SaveOptions::set_includeText(bool val) { auto c = n_.find_child("includeText"); if (c.invalid()) { c = n_.append_child() << ryml::key("includeText"); } detail::SetPrimitive(c, val); }
void SaveOptions::init() { StructWrapper::init();  }
std::string_view FileEvent::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void FileEvent::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
FileChangeType FileEvent::type() { return detail::DeserializeEnum<FileChangeType>(n_.find_child("type")); }
void FileEvent::set_type(FileChangeType val) { auto c = n_.find_child("type"); if (c.invalid()) { c = n_.append_child() << ryml::key("type"); } detail::SerializeEnum(val, c); }
void FileEvent::init() { StructWrapper::init();  }
bool FileSystemWatcher::Union0::holds_string() const { return n_.has_child("TODO"); }
Pattern FileSystemWatcher::Union0::as_string() { return detail::NodeAsPrimitive<Pattern>(n_); }
void FileSystemWatcher::Union0::set_string(Pattern val) { detail::SetPrimitive(n_, val); }
bool FileSystemWatcher::Union0::holds_RelativePattern() const { return n_.has_child("TODO"); }
RelativePattern FileSystemWatcher::Union0::as_RelativePattern() { return {n_}; }
FileSystemWatcher::Union0 FileSystemWatcher::globPattern() { return n_.find_child("globPattern"); }
WatchKind FileSystemWatcher::kind() { return detail::DeserializeEnum<WatchKind>(n_.find_child("kind")); }
bool FileSystemWatcher::has_kind() const { return n_.has_child("kind"); }
void FileSystemWatcher::set_kind(WatchKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
void FileSystemWatcher::init() { StructWrapper::init(); FileSystemWatcher::Union0(n_.append_child() << ryml::key("globPattern")).init(); }
Range Diagnostic::range() { return n_.find_child("range"); }
DiagnosticSeverity Diagnostic::severity() { return detail::DeserializeEnum<DiagnosticSeverity>(n_.find_child("severity")); }
bool Diagnostic::has_severity() const { return n_.has_child("severity"); }
void Diagnostic::set_severity(DiagnosticSeverity val) { auto c = n_.find_child("severity"); if (c.invalid()) { c = n_.append_child() << ryml::key("severity"); } detail::SerializeEnum(val, c); }
bool Diagnostic::Union2::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t Diagnostic::Union2::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void Diagnostic::Union2::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool Diagnostic::Union2::holds_string() const { return n_.has_child("TODO"); }
std::string_view Diagnostic::Union2::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void Diagnostic::Union2::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
Diagnostic::Union2 Diagnostic::code() { return n_.find_child("code"); }
bool Diagnostic::has_code() const { return n_.has_child("code"); }
Diagnostic::Union2 Diagnostic::add_code() { auto w = Diagnostic::Union2(n_.append_child() << ryml::key("code")); w.init(); return w; }
CodeDescription Diagnostic::codeDescription() { return n_.find_child("codeDescription"); }
bool Diagnostic::has_codeDescription() const { return n_.has_child("codeDescription"); }
CodeDescription Diagnostic::add_codeDescription() { auto w = CodeDescription(n_.append_child() << ryml::key("codeDescription")); w.init(); return w; }
std::string_view Diagnostic::source() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("source")); }
bool Diagnostic::has_source() const { return n_.has_child("source"); }
void Diagnostic::set_source(std::string_view val) { auto c = n_.find_child("source"); if (c.invalid()) { c = n_.append_child() << ryml::key("source"); } detail::SetPrimitive(c, val); }
std::string_view Diagnostic::message() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("message")); }
void Diagnostic::set_message(std::string_view val) { auto c = n_.find_child("message"); if (c.invalid()) { c = n_.append_child() << ryml::key("message"); } detail::SetPrimitive(c, val); }
List<DiagnosticTag> Diagnostic::tags() { return n_.find_child("tags"); }
bool Diagnostic::has_tags() const { return n_.has_child("tags"); }
List<DiagnosticTag> Diagnostic::add_tags() { auto w = List<DiagnosticTag>(n_.append_child() << ryml::key("tags")); w.init(); return w; }
List<DiagnosticRelatedInformation> Diagnostic::relatedInformation() { return n_.find_child("relatedInformation"); }
bool Diagnostic::has_relatedInformation() const { return n_.has_child("relatedInformation"); }
List<DiagnosticRelatedInformation> Diagnostic::add_relatedInformation() { auto w = List<DiagnosticRelatedInformation>(n_.append_child() << ryml::key("relatedInformation")); w.init(); return w; }
bool Diagnostic::Union8::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject Diagnostic::Union8::as_reference_Map0() {  }
bool Diagnostic::Union8::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray Diagnostic::Union8::as_reference_LSPAny() { return {n_}; }
bool Diagnostic::Union8::holds_string() const { return n_.has_child("TODO"); }
std::string_view Diagnostic::Union8::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void Diagnostic::Union8::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool Diagnostic::Union8::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t Diagnostic::Union8::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void Diagnostic::Union8::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool Diagnostic::Union8::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t Diagnostic::Union8::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void Diagnostic::Union8::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool Diagnostic::Union8::holds_decimal() const { return n_.has_child("TODO"); }
double Diagnostic::Union8::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void Diagnostic::Union8::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool Diagnostic::Union8::holds_boolean() const { return n_.has_child("TODO"); }
bool Diagnostic::Union8::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void Diagnostic::Union8::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
Diagnostic::Union8 Diagnostic::data() { return n_.find_child("data"); }
bool Diagnostic::has_data() const { return n_.has_child("data"); }
Diagnostic::Union8 Diagnostic::add_data() { auto w = Diagnostic::Union8(n_.append_child() << ryml::key("data")); w.init(); return w; }
void Diagnostic::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); }
CompletionTriggerKind CompletionContext::triggerKind() { return detail::DeserializeEnum<CompletionTriggerKind>(n_.find_child("triggerKind")); }
void CompletionContext::set_triggerKind(CompletionTriggerKind val) { auto c = n_.find_child("triggerKind"); if (c.invalid()) { c = n_.append_child() << ryml::key("triggerKind"); } detail::SerializeEnum(val, c); }
std::string_view CompletionContext::triggerCharacter() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("triggerCharacter")); }
bool CompletionContext::has_triggerCharacter() const { return n_.has_child("triggerCharacter"); }
void CompletionContext::set_triggerCharacter(std::string_view val) { auto c = n_.find_child("triggerCharacter"); if (c.invalid()) { c = n_.append_child() << ryml::key("triggerCharacter"); } detail::SetPrimitive(c, val); }
void CompletionContext::init() { StructWrapper::init();  }
std::string_view CompletionItemLabelDetails::detail() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("detail")); }
bool CompletionItemLabelDetails::has_detail() const { return n_.has_child("detail"); }
void CompletionItemLabelDetails::set_detail(std::string_view val) { auto c = n_.find_child("detail"); if (c.invalid()) { c = n_.append_child() << ryml::key("detail"); } detail::SetPrimitive(c, val); }
std::string_view CompletionItemLabelDetails::description() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("description")); }
bool CompletionItemLabelDetails::has_description() const { return n_.has_child("description"); }
void CompletionItemLabelDetails::set_description(std::string_view val) { auto c = n_.find_child("description"); if (c.invalid()) { c = n_.append_child() << ryml::key("description"); } detail::SetPrimitive(c, val); }
void CompletionItemLabelDetails::init() { StructWrapper::init();  }
std::string_view InsertReplaceEdit::newText() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("newText")); }
void InsertReplaceEdit::set_newText(std::string_view val) { auto c = n_.find_child("newText"); if (c.invalid()) { c = n_.append_child() << ryml::key("newText"); } detail::SetPrimitive(c, val); }
Range InsertReplaceEdit::insert() { return n_.find_child("insert"); }
Range InsertReplaceEdit::replace() { return n_.find_child("replace"); }
void InsertReplaceEdit::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("insert")).init(); Range(n_.append_child() << ryml::key("replace")).init(); }
List<std::string_view> CompletionItemDefaults::commitCharacters() { return n_.find_child("commitCharacters"); }
bool CompletionItemDefaults::has_commitCharacters() const { return n_.has_child("commitCharacters"); }
List<std::string_view> CompletionItemDefaults::add_commitCharacters() { auto w = List<std::string_view>(n_.append_child() << ryml::key("commitCharacters")); w.init(); return w; }
bool CompletionItemDefaults::Union1::holds_Range() const { return n_.has_child("TODO"); }
Range CompletionItemDefaults::Union1::as_Range() { return {n_}; }
bool CompletionItemDefaults::Union1::holds_EditRangeWithInsertReplace() const { return n_.has_child("TODO"); }
EditRangeWithInsertReplace CompletionItemDefaults::Union1::as_EditRangeWithInsertReplace() { return {n_}; }
CompletionItemDefaults::Union1 CompletionItemDefaults::editRange() { return n_.find_child("editRange"); }
bool CompletionItemDefaults::has_editRange() const { return n_.has_child("editRange"); }
CompletionItemDefaults::Union1 CompletionItemDefaults::add_editRange() { auto w = CompletionItemDefaults::Union1(n_.append_child() << ryml::key("editRange")); w.init(); return w; }
InsertTextFormat CompletionItemDefaults::insertTextFormat() { return detail::DeserializeEnum<InsertTextFormat>(n_.find_child("insertTextFormat")); }
bool CompletionItemDefaults::has_insertTextFormat() const { return n_.has_child("insertTextFormat"); }
void CompletionItemDefaults::set_insertTextFormat(InsertTextFormat val) { auto c = n_.find_child("insertTextFormat"); if (c.invalid()) { c = n_.append_child() << ryml::key("insertTextFormat"); } detail::SerializeEnum(val, c); }
InsertTextMode CompletionItemDefaults::insertTextMode() { return detail::DeserializeEnum<InsertTextMode>(n_.find_child("insertTextMode")); }
bool CompletionItemDefaults::has_insertTextMode() const { return n_.has_child("insertTextMode"); }
void CompletionItemDefaults::set_insertTextMode(InsertTextMode val) { auto c = n_.find_child("insertTextMode"); if (c.invalid()) { c = n_.append_child() << ryml::key("insertTextMode"); } detail::SerializeEnum(val, c); }
bool CompletionItemDefaults::Union4::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject CompletionItemDefaults::Union4::as_reference_Map0() {  }
bool CompletionItemDefaults::Union4::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray CompletionItemDefaults::Union4::as_reference_LSPAny() { return {n_}; }
bool CompletionItemDefaults::Union4::holds_string() const { return n_.has_child("TODO"); }
std::string_view CompletionItemDefaults::Union4::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void CompletionItemDefaults::Union4::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool CompletionItemDefaults::Union4::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t CompletionItemDefaults::Union4::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void CompletionItemDefaults::Union4::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool CompletionItemDefaults::Union4::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t CompletionItemDefaults::Union4::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void CompletionItemDefaults::Union4::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool CompletionItemDefaults::Union4::holds_decimal() const { return n_.has_child("TODO"); }
double CompletionItemDefaults::Union4::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void CompletionItemDefaults::Union4::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool CompletionItemDefaults::Union4::holds_boolean() const { return n_.has_child("TODO"); }
bool CompletionItemDefaults::Union4::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void CompletionItemDefaults::Union4::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
CompletionItemDefaults::Union4 CompletionItemDefaults::data() { return n_.find_child("data"); }
bool CompletionItemDefaults::has_data() const { return n_.has_child("data"); }
CompletionItemDefaults::Union4 CompletionItemDefaults::add_data() { auto w = CompletionItemDefaults::Union4(n_.append_child() << ryml::key("data")); w.init(); return w; }
void CompletionItemDefaults::init() { StructWrapper::init();  }
ApplyKind CompletionItemApplyKinds::commitCharacters() { return detail::DeserializeEnum<ApplyKind>(n_.find_child("commitCharacters")); }
bool CompletionItemApplyKinds::has_commitCharacters() const { return n_.has_child("commitCharacters"); }
void CompletionItemApplyKinds::set_commitCharacters(ApplyKind val) { auto c = n_.find_child("commitCharacters"); if (c.invalid()) { c = n_.append_child() << ryml::key("commitCharacters"); } detail::SerializeEnum(val, c); }
ApplyKind CompletionItemApplyKinds::data() { return detail::DeserializeEnum<ApplyKind>(n_.find_child("data")); }
bool CompletionItemApplyKinds::has_data() const { return n_.has_child("data"); }
void CompletionItemApplyKinds::set_data(ApplyKind val) { auto c = n_.find_child("data"); if (c.invalid()) { c = n_.append_child() << ryml::key("data"); } detail::SerializeEnum(val, c); }
void CompletionItemApplyKinds::init() { StructWrapper::init();  }
List<std::string_view> CompletionOptions::triggerCharacters() { return n_.find_child("triggerCharacters"); }
bool CompletionOptions::has_triggerCharacters() const { return n_.has_child("triggerCharacters"); }
List<std::string_view> CompletionOptions::add_triggerCharacters() { auto w = List<std::string_view>(n_.append_child() << ryml::key("triggerCharacters")); w.init(); return w; }
List<std::string_view> CompletionOptions::allCommitCharacters() { return n_.find_child("allCommitCharacters"); }
bool CompletionOptions::has_allCommitCharacters() const { return n_.has_child("allCommitCharacters"); }
List<std::string_view> CompletionOptions::add_allCommitCharacters() { auto w = List<std::string_view>(n_.append_child() << ryml::key("allCommitCharacters")); w.init(); return w; }
bool CompletionOptions::resolveProvider() { return detail::NodeAsPrimitive<bool>(n_.find_child("resolveProvider")); }
bool CompletionOptions::has_resolveProvider() const { return n_.has_child("resolveProvider"); }
void CompletionOptions::set_resolveProvider(bool val) { auto c = n_.find_child("resolveProvider"); if (c.invalid()) { c = n_.append_child() << ryml::key("resolveProvider"); } detail::SetPrimitive(c, val); }
ServerCompletionItemOptions CompletionOptions::completionItem() { return n_.find_child("completionItem"); }
bool CompletionOptions::has_completionItem() const { return n_.has_child("completionItem"); }
ServerCompletionItemOptions CompletionOptions::add_completionItem() { auto w = ServerCompletionItemOptions(n_.append_child() << ryml::key("completionItem")); w.init(); return w; }
bool CompletionOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool CompletionOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void CompletionOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void CompletionOptions::init() { StructWrapper::init();  }
bool HoverOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool HoverOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void HoverOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void HoverOptions::init() { StructWrapper::init();  }
SignatureHelpTriggerKind SignatureHelpContext::triggerKind() { return detail::DeserializeEnum<SignatureHelpTriggerKind>(n_.find_child("triggerKind")); }
void SignatureHelpContext::set_triggerKind(SignatureHelpTriggerKind val) { auto c = n_.find_child("triggerKind"); if (c.invalid()) { c = n_.append_child() << ryml::key("triggerKind"); } detail::SerializeEnum(val, c); }
std::string_view SignatureHelpContext::triggerCharacter() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("triggerCharacter")); }
bool SignatureHelpContext::has_triggerCharacter() const { return n_.has_child("triggerCharacter"); }
void SignatureHelpContext::set_triggerCharacter(std::string_view val) { auto c = n_.find_child("triggerCharacter"); if (c.invalid()) { c = n_.append_child() << ryml::key("triggerCharacter"); } detail::SetPrimitive(c, val); }
bool SignatureHelpContext::isRetrigger() { return detail::NodeAsPrimitive<bool>(n_.find_child("isRetrigger")); }
void SignatureHelpContext::set_isRetrigger(bool val) { auto c = n_.find_child("isRetrigger"); if (c.invalid()) { c = n_.append_child() << ryml::key("isRetrigger"); } detail::SetPrimitive(c, val); }
SignatureHelp SignatureHelpContext::activeSignatureHelp() { return n_.find_child("activeSignatureHelp"); }
bool SignatureHelpContext::has_activeSignatureHelp() const { return n_.has_child("activeSignatureHelp"); }
SignatureHelp SignatureHelpContext::add_activeSignatureHelp() { auto w = SignatureHelp(n_.append_child() << ryml::key("activeSignatureHelp")); w.init(); return w; }
void SignatureHelpContext::init() { StructWrapper::init();  }
std::string_view SignatureInformation::label() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("label")); }
void SignatureInformation::set_label(std::string_view val) { auto c = n_.find_child("label"); if (c.invalid()) { c = n_.append_child() << ryml::key("label"); } detail::SetPrimitive(c, val); }
bool SignatureInformation::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view SignatureInformation::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void SignatureInformation::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool SignatureInformation::Union1::holds_MarkupContent() const { return n_.has_child("TODO"); }
MarkupContent SignatureInformation::Union1::as_MarkupContent() { return {n_}; }
SignatureInformation::Union1 SignatureInformation::documentation() { return n_.find_child("documentation"); }
bool SignatureInformation::has_documentation() const { return n_.has_child("documentation"); }
SignatureInformation::Union1 SignatureInformation::add_documentation() { auto w = SignatureInformation::Union1(n_.append_child() << ryml::key("documentation")); w.init(); return w; }
List<ParameterInformation> SignatureInformation::parameters() { return n_.find_child("parameters"); }
bool SignatureInformation::has_parameters() const { return n_.has_child("parameters"); }
List<ParameterInformation> SignatureInformation::add_parameters() { auto w = List<ParameterInformation>(n_.append_child() << ryml::key("parameters")); w.init(); return w; }
bool SignatureInformation::Union3::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t SignatureInformation::Union3::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void SignatureInformation::Union3::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
SignatureInformation::Union3 SignatureInformation::activeParameter() { return n_.find_child("activeParameter"); }
bool SignatureInformation::has_activeParameter() const { return n_.has_child("activeParameter"); }
SignatureInformation::Union3 SignatureInformation::add_activeParameter() { auto w = SignatureInformation::Union3(n_.append_child() << ryml::key("activeParameter")); w.init(); return w; }
void SignatureInformation::init() { StructWrapper::init();  }
List<std::string_view> SignatureHelpOptions::triggerCharacters() { return n_.find_child("triggerCharacters"); }
bool SignatureHelpOptions::has_triggerCharacters() const { return n_.has_child("triggerCharacters"); }
List<std::string_view> SignatureHelpOptions::add_triggerCharacters() { auto w = List<std::string_view>(n_.append_child() << ryml::key("triggerCharacters")); w.init(); return w; }
List<std::string_view> SignatureHelpOptions::retriggerCharacters() { return n_.find_child("retriggerCharacters"); }
bool SignatureHelpOptions::has_retriggerCharacters() const { return n_.has_child("retriggerCharacters"); }
List<std::string_view> SignatureHelpOptions::add_retriggerCharacters() { auto w = List<std::string_view>(n_.append_child() << ryml::key("retriggerCharacters")); w.init(); return w; }
bool SignatureHelpOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool SignatureHelpOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void SignatureHelpOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void SignatureHelpOptions::init() { StructWrapper::init();  }
bool DefinitionOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool DefinitionOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void DefinitionOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void DefinitionOptions::init() { StructWrapper::init();  }
bool ReferenceContext::includeDeclaration() { return detail::NodeAsPrimitive<bool>(n_.find_child("includeDeclaration")); }
void ReferenceContext::set_includeDeclaration(bool val) { auto c = n_.find_child("includeDeclaration"); if (c.invalid()) { c = n_.append_child() << ryml::key("includeDeclaration"); } detail::SetPrimitive(c, val); }
void ReferenceContext::init() { StructWrapper::init();  }
bool ReferenceOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool ReferenceOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void ReferenceOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void ReferenceOptions::init() { StructWrapper::init();  }
bool DocumentHighlightOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool DocumentHighlightOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void DocumentHighlightOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void DocumentHighlightOptions::init() { StructWrapper::init();  }
std::string_view BaseSymbolInformation::name() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("name")); }
void BaseSymbolInformation::set_name(std::string_view val) { auto c = n_.find_child("name"); if (c.invalid()) { c = n_.append_child() << ryml::key("name"); } detail::SetPrimitive(c, val); }
SymbolKind BaseSymbolInformation::kind() { return detail::DeserializeEnum<SymbolKind>(n_.find_child("kind")); }
void BaseSymbolInformation::set_kind(SymbolKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
List<SymbolTag> BaseSymbolInformation::tags() { return n_.find_child("tags"); }
bool BaseSymbolInformation::has_tags() const { return n_.has_child("tags"); }
List<SymbolTag> BaseSymbolInformation::add_tags() { auto w = List<SymbolTag>(n_.append_child() << ryml::key("tags")); w.init(); return w; }
std::string_view BaseSymbolInformation::containerName() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("containerName")); }
bool BaseSymbolInformation::has_containerName() const { return n_.has_child("containerName"); }
void BaseSymbolInformation::set_containerName(std::string_view val) { auto c = n_.find_child("containerName"); if (c.invalid()) { c = n_.append_child() << ryml::key("containerName"); } detail::SetPrimitive(c, val); }
void BaseSymbolInformation::init() { StructWrapper::init();  }
std::string_view DocumentSymbolOptions::label() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("label")); }
bool DocumentSymbolOptions::has_label() const { return n_.has_child("label"); }
void DocumentSymbolOptions::set_label(std::string_view val) { auto c = n_.find_child("label"); if (c.invalid()) { c = n_.append_child() << ryml::key("label"); } detail::SetPrimitive(c, val); }
bool DocumentSymbolOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool DocumentSymbolOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void DocumentSymbolOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void DocumentSymbolOptions::init() { StructWrapper::init();  }
List<Diagnostic> CodeActionContext::diagnostics() { return n_.find_child("diagnostics"); }
List<CodeActionKind> CodeActionContext::only() { return n_.find_child("only"); }
bool CodeActionContext::has_only() const { return n_.has_child("only"); }
List<CodeActionKind> CodeActionContext::add_only() { auto w = List<CodeActionKind>(n_.append_child() << ryml::key("only")); w.init(); return w; }
CodeActionTriggerKind CodeActionContext::triggerKind() { return detail::DeserializeEnum<CodeActionTriggerKind>(n_.find_child("triggerKind")); }
bool CodeActionContext::has_triggerKind() const { return n_.has_child("triggerKind"); }
void CodeActionContext::set_triggerKind(CodeActionTriggerKind val) { auto c = n_.find_child("triggerKind"); if (c.invalid()) { c = n_.append_child() << ryml::key("triggerKind"); } detail::SerializeEnum(val, c); }
void CodeActionContext::init() { StructWrapper::init(); List<Diagnostic>(n_.append_child() << ryml::key("diagnostics")).init(); }
std::string_view CodeActionDisabled::reason() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("reason")); }
void CodeActionDisabled::set_reason(std::string_view val) { auto c = n_.find_child("reason"); if (c.invalid()) { c = n_.append_child() << ryml::key("reason"); } detail::SetPrimitive(c, val); }
void CodeActionDisabled::init() { StructWrapper::init();  }
List<CodeActionKind> CodeActionOptions::codeActionKinds() { return n_.find_child("codeActionKinds"); }
bool CodeActionOptions::has_codeActionKinds() const { return n_.has_child("codeActionKinds"); }
List<CodeActionKind> CodeActionOptions::add_codeActionKinds() { auto w = List<CodeActionKind>(n_.append_child() << ryml::key("codeActionKinds")); w.init(); return w; }
List<CodeActionKindDocumentation> CodeActionOptions::documentation() { return n_.find_child("documentation"); }
bool CodeActionOptions::has_documentation() const { return n_.has_child("documentation"); }
List<CodeActionKindDocumentation> CodeActionOptions::add_documentation() { auto w = List<CodeActionKindDocumentation>(n_.append_child() << ryml::key("documentation")); w.init(); return w; }
bool CodeActionOptions::resolveProvider() { return detail::NodeAsPrimitive<bool>(n_.find_child("resolveProvider")); }
bool CodeActionOptions::has_resolveProvider() const { return n_.has_child("resolveProvider"); }
void CodeActionOptions::set_resolveProvider(bool val) { auto c = n_.find_child("resolveProvider"); if (c.invalid()) { c = n_.append_child() << ryml::key("resolveProvider"); } detail::SetPrimitive(c, val); }
bool CodeActionOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool CodeActionOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void CodeActionOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void CodeActionOptions::init() { StructWrapper::init();  }
std::string_view LocationUriOnly::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void LocationUriOnly::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
void LocationUriOnly::init() { StructWrapper::init();  }
bool WorkspaceSymbolOptions::resolveProvider() { return detail::NodeAsPrimitive<bool>(n_.find_child("resolveProvider")); }
bool WorkspaceSymbolOptions::has_resolveProvider() const { return n_.has_child("resolveProvider"); }
void WorkspaceSymbolOptions::set_resolveProvider(bool val) { auto c = n_.find_child("resolveProvider"); if (c.invalid()) { c = n_.append_child() << ryml::key("resolveProvider"); } detail::SetPrimitive(c, val); }
bool WorkspaceSymbolOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool WorkspaceSymbolOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void WorkspaceSymbolOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void WorkspaceSymbolOptions::init() { StructWrapper::init();  }
bool CodeLensOptions::resolveProvider() { return detail::NodeAsPrimitive<bool>(n_.find_child("resolveProvider")); }
bool CodeLensOptions::has_resolveProvider() const { return n_.has_child("resolveProvider"); }
void CodeLensOptions::set_resolveProvider(bool val) { auto c = n_.find_child("resolveProvider"); if (c.invalid()) { c = n_.append_child() << ryml::key("resolveProvider"); } detail::SetPrimitive(c, val); }
bool CodeLensOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool CodeLensOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void CodeLensOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void CodeLensOptions::init() { StructWrapper::init();  }
bool DocumentLinkOptions::resolveProvider() { return detail::NodeAsPrimitive<bool>(n_.find_child("resolveProvider")); }
bool DocumentLinkOptions::has_resolveProvider() const { return n_.has_child("resolveProvider"); }
void DocumentLinkOptions::set_resolveProvider(bool val) { auto c = n_.find_child("resolveProvider"); if (c.invalid()) { c = n_.append_child() << ryml::key("resolveProvider"); } detail::SetPrimitive(c, val); }
bool DocumentLinkOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool DocumentLinkOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void DocumentLinkOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void DocumentLinkOptions::init() { StructWrapper::init();  }
std::uint32_t FormattingOptions::tabSize() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("tabSize")); }
void FormattingOptions::set_tabSize(std::uint32_t val) { auto c = n_.find_child("tabSize"); if (c.invalid()) { c = n_.append_child() << ryml::key("tabSize"); } detail::SetPrimitive(c, val); }
bool FormattingOptions::insertSpaces() { return detail::NodeAsPrimitive<bool>(n_.find_child("insertSpaces")); }
void FormattingOptions::set_insertSpaces(bool val) { auto c = n_.find_child("insertSpaces"); if (c.invalid()) { c = n_.append_child() << ryml::key("insertSpaces"); } detail::SetPrimitive(c, val); }
bool FormattingOptions::trimTrailingWhitespace() { return detail::NodeAsPrimitive<bool>(n_.find_child("trimTrailingWhitespace")); }
bool FormattingOptions::has_trimTrailingWhitespace() const { return n_.has_child("trimTrailingWhitespace"); }
void FormattingOptions::set_trimTrailingWhitespace(bool val) { auto c = n_.find_child("trimTrailingWhitespace"); if (c.invalid()) { c = n_.append_child() << ryml::key("trimTrailingWhitespace"); } detail::SetPrimitive(c, val); }
bool FormattingOptions::insertFinalNewline() { return detail::NodeAsPrimitive<bool>(n_.find_child("insertFinalNewline")); }
bool FormattingOptions::has_insertFinalNewline() const { return n_.has_child("insertFinalNewline"); }
void FormattingOptions::set_insertFinalNewline(bool val) { auto c = n_.find_child("insertFinalNewline"); if (c.invalid()) { c = n_.append_child() << ryml::key("insertFinalNewline"); } detail::SetPrimitive(c, val); }
bool FormattingOptions::trimFinalNewlines() { return detail::NodeAsPrimitive<bool>(n_.find_child("trimFinalNewlines")); }
bool FormattingOptions::has_trimFinalNewlines() const { return n_.has_child("trimFinalNewlines"); }
void FormattingOptions::set_trimFinalNewlines(bool val) { auto c = n_.find_child("trimFinalNewlines"); if (c.invalid()) { c = n_.append_child() << ryml::key("trimFinalNewlines"); } detail::SetPrimitive(c, val); }
void FormattingOptions::init() { StructWrapper::init();  }
bool DocumentFormattingOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool DocumentFormattingOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void DocumentFormattingOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void DocumentFormattingOptions::init() { StructWrapper::init();  }
bool DocumentRangeFormattingOptions::rangesSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("rangesSupport")); }
bool DocumentRangeFormattingOptions::has_rangesSupport() const { return n_.has_child("rangesSupport"); }
void DocumentRangeFormattingOptions::set_rangesSupport(bool val) { auto c = n_.find_child("rangesSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("rangesSupport"); } detail::SetPrimitive(c, val); }
bool DocumentRangeFormattingOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool DocumentRangeFormattingOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void DocumentRangeFormattingOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void DocumentRangeFormattingOptions::init() { StructWrapper::init();  }
std::string_view DocumentOnTypeFormattingOptions::firstTriggerCharacter() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("firstTriggerCharacter")); }
void DocumentOnTypeFormattingOptions::set_firstTriggerCharacter(std::string_view val) { auto c = n_.find_child("firstTriggerCharacter"); if (c.invalid()) { c = n_.append_child() << ryml::key("firstTriggerCharacter"); } detail::SetPrimitive(c, val); }
List<std::string_view> DocumentOnTypeFormattingOptions::moreTriggerCharacter() { return n_.find_child("moreTriggerCharacter"); }
bool DocumentOnTypeFormattingOptions::has_moreTriggerCharacter() const { return n_.has_child("moreTriggerCharacter"); }
List<std::string_view> DocumentOnTypeFormattingOptions::add_moreTriggerCharacter() { auto w = List<std::string_view>(n_.append_child() << ryml::key("moreTriggerCharacter")); w.init(); return w; }
void DocumentOnTypeFormattingOptions::init() { StructWrapper::init();  }
bool RenameOptions::prepareProvider() { return detail::NodeAsPrimitive<bool>(n_.find_child("prepareProvider")); }
bool RenameOptions::has_prepareProvider() const { return n_.has_child("prepareProvider"); }
void RenameOptions::set_prepareProvider(bool val) { auto c = n_.find_child("prepareProvider"); if (c.invalid()) { c = n_.append_child() << ryml::key("prepareProvider"); } detail::SetPrimitive(c, val); }
bool RenameOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool RenameOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void RenameOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void RenameOptions::init() { StructWrapper::init();  }
Range PrepareRenamePlaceholder::range() { return n_.find_child("range"); }
std::string_view PrepareRenamePlaceholder::placeholder() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("placeholder")); }
void PrepareRenamePlaceholder::set_placeholder(std::string_view val) { auto c = n_.find_child("placeholder"); if (c.invalid()) { c = n_.append_child() << ryml::key("placeholder"); } detail::SetPrimitive(c, val); }
void PrepareRenamePlaceholder::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); }
bool PrepareRenameDefaultBehavior::defaultBehavior() { return detail::NodeAsPrimitive<bool>(n_.find_child("defaultBehavior")); }
void PrepareRenameDefaultBehavior::set_defaultBehavior(bool val) { auto c = n_.find_child("defaultBehavior"); if (c.invalid()) { c = n_.append_child() << ryml::key("defaultBehavior"); } detail::SetPrimitive(c, val); }
void PrepareRenameDefaultBehavior::init() { StructWrapper::init();  }
List<std::string_view> ExecuteCommandOptions::commands() { return n_.find_child("commands"); }
bool ExecuteCommandOptions::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool ExecuteCommandOptions::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void ExecuteCommandOptions::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
void ExecuteCommandOptions::init() { StructWrapper::init(); List<std::string_view>(n_.append_child() << ryml::key("commands")).init(); }
bool WorkspaceEditMetadata::isRefactoring() { return detail::NodeAsPrimitive<bool>(n_.find_child("isRefactoring")); }
bool WorkspaceEditMetadata::has_isRefactoring() const { return n_.has_child("isRefactoring"); }
void WorkspaceEditMetadata::set_isRefactoring(bool val) { auto c = n_.find_child("isRefactoring"); if (c.invalid()) { c = n_.append_child() << ryml::key("isRefactoring"); } detail::SetPrimitive(c, val); }
void WorkspaceEditMetadata::init() { StructWrapper::init();  }
List<std::string_view> SemanticTokensLegend::tokenTypes() { return n_.find_child("tokenTypes"); }
List<std::string_view> SemanticTokensLegend::tokenModifiers() { return n_.find_child("tokenModifiers"); }
void SemanticTokensLegend::init() { StructWrapper::init(); List<std::string_view>(n_.append_child() << ryml::key("tokenTypes")).init(); List<std::string_view>(n_.append_child() << ryml::key("tokenModifiers")).init(); }
bool SemanticTokensFullDelta::delta() { return detail::NodeAsPrimitive<bool>(n_.find_child("delta")); }
bool SemanticTokensFullDelta::has_delta() const { return n_.has_child("delta"); }
void SemanticTokensFullDelta::set_delta(bool val) { auto c = n_.find_child("delta"); if (c.invalid()) { c = n_.append_child() << ryml::key("delta"); } detail::SetPrimitive(c, val); }
void SemanticTokensFullDelta::init() { StructWrapper::init();  }
bool OptionalVersionedTextDocumentIdentifier::Union0::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t OptionalVersionedTextDocumentIdentifier::Union0::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void OptionalVersionedTextDocumentIdentifier::Union0::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
OptionalVersionedTextDocumentIdentifier::Union0 OptionalVersionedTextDocumentIdentifier::version() { return n_.find_child("version"); }
std::string_view OptionalVersionedTextDocumentIdentifier::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void OptionalVersionedTextDocumentIdentifier::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
void OptionalVersionedTextDocumentIdentifier::init() { StructWrapper::init(); OptionalVersionedTextDocumentIdentifier::Union0(n_.append_child() << ryml::key("version")).init(); }
ChangeAnnotationIdentifier AnnotatedTextEdit::annotationId() { return detail::NodeAsPrimitive<ChangeAnnotationIdentifier>(n_.find_child("annotationId")); }
void AnnotatedTextEdit::set_annotationId(ChangeAnnotationIdentifier val) { auto c = n_.find_child("annotationId"); if (c.invalid()) { c = n_.append_child() << ryml::key("annotationId"); } detail::SetPrimitive(c, val); }
Range AnnotatedTextEdit::range() { return n_.find_child("range"); }
std::string_view AnnotatedTextEdit::newText() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("newText")); }
void AnnotatedTextEdit::set_newText(std::string_view val) { auto c = n_.find_child("newText"); if (c.invalid()) { c = n_.append_child() << ryml::key("newText"); } detail::SetPrimitive(c, val); }
void AnnotatedTextEdit::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); }
Range SnippetTextEdit::range() { return n_.find_child("range"); }
StringValue SnippetTextEdit::snippet() { return n_.find_child("snippet"); }
ChangeAnnotationIdentifier SnippetTextEdit::annotationId() { return detail::NodeAsPrimitive<ChangeAnnotationIdentifier>(n_.find_child("annotationId")); }
bool SnippetTextEdit::has_annotationId() const { return n_.has_child("annotationId"); }
void SnippetTextEdit::set_annotationId(ChangeAnnotationIdentifier val) { auto c = n_.find_child("annotationId"); if (c.invalid()) { c = n_.append_child() << ryml::key("annotationId"); } detail::SetPrimitive(c, val); }
void SnippetTextEdit::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); StringValue(n_.append_child() << ryml::key("snippet")).init(); }
std::string_view ResourceOperation::kind() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("kind")); }
void ResourceOperation::set_kind(std::string_view val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SetPrimitive(c, val); }
ChangeAnnotationIdentifier ResourceOperation::annotationId() { return detail::NodeAsPrimitive<ChangeAnnotationIdentifier>(n_.find_child("annotationId")); }
bool ResourceOperation::has_annotationId() const { return n_.has_child("annotationId"); }
void ResourceOperation::set_annotationId(ChangeAnnotationIdentifier val) { auto c = n_.find_child("annotationId"); if (c.invalid()) { c = n_.append_child() << ryml::key("annotationId"); } detail::SetPrimitive(c, val); }
void ResourceOperation::init() { StructWrapper::init();  }
bool CreateFileOptions::overwrite() { return detail::NodeAsPrimitive<bool>(n_.find_child("overwrite")); }
bool CreateFileOptions::has_overwrite() const { return n_.has_child("overwrite"); }
void CreateFileOptions::set_overwrite(bool val) { auto c = n_.find_child("overwrite"); if (c.invalid()) { c = n_.append_child() << ryml::key("overwrite"); } detail::SetPrimitive(c, val); }
bool CreateFileOptions::ignoreIfExists() { return detail::NodeAsPrimitive<bool>(n_.find_child("ignoreIfExists")); }
bool CreateFileOptions::has_ignoreIfExists() const { return n_.has_child("ignoreIfExists"); }
void CreateFileOptions::set_ignoreIfExists(bool val) { auto c = n_.find_child("ignoreIfExists"); if (c.invalid()) { c = n_.append_child() << ryml::key("ignoreIfExists"); } detail::SetPrimitive(c, val); }
void CreateFileOptions::init() { StructWrapper::init();  }
bool RenameFileOptions::overwrite() { return detail::NodeAsPrimitive<bool>(n_.find_child("overwrite")); }
bool RenameFileOptions::has_overwrite() const { return n_.has_child("overwrite"); }
void RenameFileOptions::set_overwrite(bool val) { auto c = n_.find_child("overwrite"); if (c.invalid()) { c = n_.append_child() << ryml::key("overwrite"); } detail::SetPrimitive(c, val); }
bool RenameFileOptions::ignoreIfExists() { return detail::NodeAsPrimitive<bool>(n_.find_child("ignoreIfExists")); }
bool RenameFileOptions::has_ignoreIfExists() const { return n_.has_child("ignoreIfExists"); }
void RenameFileOptions::set_ignoreIfExists(bool val) { auto c = n_.find_child("ignoreIfExists"); if (c.invalid()) { c = n_.append_child() << ryml::key("ignoreIfExists"); } detail::SetPrimitive(c, val); }
void RenameFileOptions::init() { StructWrapper::init();  }
bool DeleteFileOptions::recursive() { return detail::NodeAsPrimitive<bool>(n_.find_child("recursive")); }
bool DeleteFileOptions::has_recursive() const { return n_.has_child("recursive"); }
void DeleteFileOptions::set_recursive(bool val) { auto c = n_.find_child("recursive"); if (c.invalid()) { c = n_.append_child() << ryml::key("recursive"); } detail::SetPrimitive(c, val); }
bool DeleteFileOptions::ignoreIfNotExists() { return detail::NodeAsPrimitive<bool>(n_.find_child("ignoreIfNotExists")); }
bool DeleteFileOptions::has_ignoreIfNotExists() const { return n_.has_child("ignoreIfNotExists"); }
void DeleteFileOptions::set_ignoreIfNotExists(bool val) { auto c = n_.find_child("ignoreIfNotExists"); if (c.invalid()) { c = n_.append_child() << ryml::key("ignoreIfNotExists"); } detail::SetPrimitive(c, val); }
void DeleteFileOptions::init() { StructWrapper::init();  }
std::string_view FileOperationPattern::glob() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("glob")); }
void FileOperationPattern::set_glob(std::string_view val) { auto c = n_.find_child("glob"); if (c.invalid()) { c = n_.append_child() << ryml::key("glob"); } detail::SetPrimitive(c, val); }
FileOperationPatternKind FileOperationPattern::matches() { return detail::DeserializeEnum<FileOperationPatternKind>(n_.find_child("matches")); }
bool FileOperationPattern::has_matches() const { return n_.has_child("matches"); }
void FileOperationPattern::set_matches(FileOperationPatternKind val) { auto c = n_.find_child("matches"); if (c.invalid()) { c = n_.append_child() << ryml::key("matches"); } detail::SerializeEnum(val, c); }
FileOperationPatternOptions FileOperationPattern::options() { return n_.find_child("options"); }
bool FileOperationPattern::has_options() const { return n_.has_child("options"); }
FileOperationPatternOptions FileOperationPattern::add_options() { auto w = FileOperationPatternOptions(n_.append_child() << ryml::key("options")); w.init(); return w; }
void FileOperationPattern::init() { StructWrapper::init();  }
std::string_view WorkspaceFullDocumentDiagnosticReport::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void WorkspaceFullDocumentDiagnosticReport::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
bool WorkspaceFullDocumentDiagnosticReport::Union1::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t WorkspaceFullDocumentDiagnosticReport::Union1::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void WorkspaceFullDocumentDiagnosticReport::Union1::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
WorkspaceFullDocumentDiagnosticReport::Union1 WorkspaceFullDocumentDiagnosticReport::version() { return n_.find_child("version"); }
std::string_view WorkspaceFullDocumentDiagnosticReport::resultId() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("resultId")); }
bool WorkspaceFullDocumentDiagnosticReport::has_resultId() const { return n_.has_child("resultId"); }
void WorkspaceFullDocumentDiagnosticReport::set_resultId(std::string_view val) { auto c = n_.find_child("resultId"); if (c.invalid()) { c = n_.append_child() << ryml::key("resultId"); } detail::SetPrimitive(c, val); }
List<Diagnostic> WorkspaceFullDocumentDiagnosticReport::items() { return n_.find_child("items"); }
void WorkspaceFullDocumentDiagnosticReport::init() { StructWrapper::init(); WorkspaceFullDocumentDiagnosticReport::Union1(n_.append_child() << ryml::key("version")).init(); List<Diagnostic>(n_.append_child() << ryml::key("items")).init(); }
std::string_view WorkspaceUnchangedDocumentDiagnosticReport::uri() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("uri")); }
void WorkspaceUnchangedDocumentDiagnosticReport::set_uri(std::string_view val) { auto c = n_.find_child("uri"); if (c.invalid()) { c = n_.append_child() << ryml::key("uri"); } detail::SetPrimitive(c, val); }
bool WorkspaceUnchangedDocumentDiagnosticReport::Union1::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t WorkspaceUnchangedDocumentDiagnosticReport::Union1::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void WorkspaceUnchangedDocumentDiagnosticReport::Union1::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
WorkspaceUnchangedDocumentDiagnosticReport::Union1 WorkspaceUnchangedDocumentDiagnosticReport::version() { return n_.find_child("version"); }
std::string_view WorkspaceUnchangedDocumentDiagnosticReport::resultId() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("resultId")); }
void WorkspaceUnchangedDocumentDiagnosticReport::set_resultId(std::string_view val) { auto c = n_.find_child("resultId"); if (c.invalid()) { c = n_.append_child() << ryml::key("resultId"); } detail::SetPrimitive(c, val); }
void WorkspaceUnchangedDocumentDiagnosticReport::init() { StructWrapper::init(); WorkspaceUnchangedDocumentDiagnosticReport::Union1(n_.append_child() << ryml::key("version")).init(); }
NotebookCellKind NotebookCell::kind() { return detail::DeserializeEnum<NotebookCellKind>(n_.find_child("kind")); }
void NotebookCell::set_kind(NotebookCellKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
std::string_view NotebookCell::document() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("document")); }
void NotebookCell::set_document(std::string_view val) { auto c = n_.find_child("document"); if (c.invalid()) { c = n_.append_child() << ryml::key("document"); } detail::SetPrimitive(c, val); }
LSPObject NotebookCell::metadata() { return n_.find_child("metadata"); }
bool NotebookCell::has_metadata() const { return n_.has_child("metadata"); }
LSPObject NotebookCell::add_metadata() { auto w = LSPObject(n_.append_child() << ryml::key("metadata")); w.init(); return w; }
ExecutionSummary NotebookCell::executionSummary() { return n_.find_child("executionSummary"); }
bool NotebookCell::has_executionSummary() const { return n_.has_child("executionSummary"); }
ExecutionSummary NotebookCell::add_executionSummary() { auto w = ExecutionSummary(n_.append_child() << ryml::key("executionSummary")); w.init(); return w; }
void NotebookCell::init() { StructWrapper::init();  }
bool NotebookDocumentFilterWithNotebook::Union0::holds_string() const { return n_.has_child("TODO"); }
std::string_view NotebookDocumentFilterWithNotebook::Union0::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void NotebookDocumentFilterWithNotebook::Union0::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool NotebookDocumentFilterWithNotebook::Union0::Union1::holds_NotebookDocumentFilterNotebookType() const { return n_.has_child("TODO"); }
NotebookDocumentFilterNotebookType NotebookDocumentFilterWithNotebook::Union0::Union1::as_NotebookDocumentFilterNotebookType() { return {n_}; }
bool NotebookDocumentFilterWithNotebook::Union0::Union1::holds_NotebookDocumentFilterScheme() const { return n_.has_child("TODO"); }
NotebookDocumentFilterScheme NotebookDocumentFilterWithNotebook::Union0::Union1::as_NotebookDocumentFilterScheme() { return {n_}; }
bool NotebookDocumentFilterWithNotebook::Union0::Union1::holds_NotebookDocumentFilterPattern() const { return n_.has_child("TODO"); }
NotebookDocumentFilterPattern NotebookDocumentFilterWithNotebook::Union0::Union1::as_NotebookDocumentFilterPattern() { return {n_}; }
bool NotebookDocumentFilterWithNotebook::Union0::holds_reference_Union1() const { return n_.has_child("TODO"); }
NotebookDocumentFilter NotebookDocumentFilterWithNotebook::Union0::as_reference_Union1() {  }
NotebookDocumentFilterWithNotebook::Union0 NotebookDocumentFilterWithNotebook::notebook() { return n_.find_child("notebook"); }
List<NotebookCellLanguage> NotebookDocumentFilterWithNotebook::cells() { return n_.find_child("cells"); }
bool NotebookDocumentFilterWithNotebook::has_cells() const { return n_.has_child("cells"); }
List<NotebookCellLanguage> NotebookDocumentFilterWithNotebook::add_cells() { auto w = List<NotebookCellLanguage>(n_.append_child() << ryml::key("cells")); w.init(); return w; }
void NotebookDocumentFilterWithNotebook::init() { StructWrapper::init(); NotebookDocumentFilterWithNotebook::Union0(n_.append_child() << ryml::key("notebook")).init(); }
bool NotebookDocumentFilterWithCells::Union0::holds_string() const { return n_.has_child("TODO"); }
std::string_view NotebookDocumentFilterWithCells::Union0::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void NotebookDocumentFilterWithCells::Union0::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool NotebookDocumentFilterWithCells::Union0::Union1::holds_NotebookDocumentFilterNotebookType() const { return n_.has_child("TODO"); }
NotebookDocumentFilterNotebookType NotebookDocumentFilterWithCells::Union0::Union1::as_NotebookDocumentFilterNotebookType() { return {n_}; }
bool NotebookDocumentFilterWithCells::Union0::Union1::holds_NotebookDocumentFilterScheme() const { return n_.has_child("TODO"); }
NotebookDocumentFilterScheme NotebookDocumentFilterWithCells::Union0::Union1::as_NotebookDocumentFilterScheme() { return {n_}; }
bool NotebookDocumentFilterWithCells::Union0::Union1::holds_NotebookDocumentFilterPattern() const { return n_.has_child("TODO"); }
NotebookDocumentFilterPattern NotebookDocumentFilterWithCells::Union0::Union1::as_NotebookDocumentFilterPattern() { return {n_}; }
bool NotebookDocumentFilterWithCells::Union0::holds_reference_Union1() const { return n_.has_child("TODO"); }
NotebookDocumentFilter NotebookDocumentFilterWithCells::Union0::as_reference_Union1() {  }
NotebookDocumentFilterWithCells::Union0 NotebookDocumentFilterWithCells::notebook() { return n_.find_child("notebook"); }
bool NotebookDocumentFilterWithCells::has_notebook() const { return n_.has_child("notebook"); }
NotebookDocumentFilterWithCells::Union0 NotebookDocumentFilterWithCells::add_notebook() { auto w = NotebookDocumentFilterWithCells::Union0(n_.append_child() << ryml::key("notebook")); w.init(); return w; }
List<NotebookCellLanguage> NotebookDocumentFilterWithCells::cells() { return n_.find_child("cells"); }
void NotebookDocumentFilterWithCells::init() { StructWrapper::init(); List<NotebookCellLanguage>(n_.append_child() << ryml::key("cells")).init(); }
NotebookDocumentCellChangeStructure NotebookDocumentCellChanges::structure() { return n_.find_child("structure"); }
bool NotebookDocumentCellChanges::has_structure() const { return n_.has_child("structure"); }
NotebookDocumentCellChangeStructure NotebookDocumentCellChanges::add_structure() { auto w = NotebookDocumentCellChangeStructure(n_.append_child() << ryml::key("structure")); w.init(); return w; }
List<NotebookCell> NotebookDocumentCellChanges::data() { return n_.find_child("data"); }
bool NotebookDocumentCellChanges::has_data() const { return n_.has_child("data"); }
List<NotebookCell> NotebookDocumentCellChanges::add_data() { auto w = List<NotebookCell>(n_.append_child() << ryml::key("data")); w.init(); return w; }
List<NotebookDocumentCellContentChanges> NotebookDocumentCellChanges::textContent() { return n_.find_child("textContent"); }
bool NotebookDocumentCellChanges::has_textContent() const { return n_.has_child("textContent"); }
List<NotebookDocumentCellContentChanges> NotebookDocumentCellChanges::add_textContent() { auto w = List<NotebookDocumentCellContentChanges>(n_.append_child() << ryml::key("textContent")); w.init(); return w; }
void NotebookDocumentCellChanges::init() { StructWrapper::init();  }
Range SelectedCompletionInfo::range() { return n_.find_child("range"); }
std::string_view SelectedCompletionInfo::text() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("text")); }
void SelectedCompletionInfo::set_text(std::string_view val) { auto c = n_.find_child("text"); if (c.invalid()) { c = n_.append_child() << ryml::key("text"); } detail::SetPrimitive(c, val); }
void SelectedCompletionInfo::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); }
std::string_view ClientInfo::name() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("name")); }
void ClientInfo::set_name(std::string_view val) { auto c = n_.find_child("name"); if (c.invalid()) { c = n_.append_child() << ryml::key("name"); } detail::SetPrimitive(c, val); }
std::string_view ClientInfo::version() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("version")); }
bool ClientInfo::has_version() const { return n_.has_child("version"); }
void ClientInfo::set_version(std::string_view val) { auto c = n_.find_child("version"); if (c.invalid()) { c = n_.append_child() << ryml::key("version"); } detail::SetPrimitive(c, val); }
void ClientInfo::init() { StructWrapper::init();  }
WorkspaceClientCapabilities ClientCapabilities::workspace() { return n_.find_child("workspace"); }
bool ClientCapabilities::has_workspace() const { return n_.has_child("workspace"); }
WorkspaceClientCapabilities ClientCapabilities::add_workspace() { auto w = WorkspaceClientCapabilities(n_.append_child() << ryml::key("workspace")); w.init(); return w; }
TextDocumentClientCapabilities ClientCapabilities::textDocument() { return n_.find_child("textDocument"); }
bool ClientCapabilities::has_textDocument() const { return n_.has_child("textDocument"); }
TextDocumentClientCapabilities ClientCapabilities::add_textDocument() { auto w = TextDocumentClientCapabilities(n_.append_child() << ryml::key("textDocument")); w.init(); return w; }
NotebookDocumentClientCapabilities ClientCapabilities::notebookDocument() { return n_.find_child("notebookDocument"); }
bool ClientCapabilities::has_notebookDocument() const { return n_.has_child("notebookDocument"); }
NotebookDocumentClientCapabilities ClientCapabilities::add_notebookDocument() { auto w = NotebookDocumentClientCapabilities(n_.append_child() << ryml::key("notebookDocument")); w.init(); return w; }
WindowClientCapabilities ClientCapabilities::window() { return n_.find_child("window"); }
bool ClientCapabilities::has_window() const { return n_.has_child("window"); }
WindowClientCapabilities ClientCapabilities::add_window() { auto w = WindowClientCapabilities(n_.append_child() << ryml::key("window")); w.init(); return w; }
GeneralClientCapabilities ClientCapabilities::general() { return n_.find_child("general"); }
bool ClientCapabilities::has_general() const { return n_.has_child("general"); }
GeneralClientCapabilities ClientCapabilities::add_general() { auto w = GeneralClientCapabilities(n_.append_child() << ryml::key("general")); w.init(); return w; }
bool ClientCapabilities::Union5::holds_reference_Map0() const { return n_.has_child("TODO"); }
LSPObject ClientCapabilities::Union5::as_reference_Map0() {  }
bool ClientCapabilities::Union5::holds_reference_LSPAny() const { return n_.has_child("TODO"); }
LSPArray ClientCapabilities::Union5::as_reference_LSPAny() { return {n_}; }
bool ClientCapabilities::Union5::holds_string() const { return n_.has_child("TODO"); }
std::string_view ClientCapabilities::Union5::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void ClientCapabilities::Union5::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool ClientCapabilities::Union5::holds_integer() const { return n_.has_child("TODO"); }
std::int32_t ClientCapabilities::Union5::as_integer() { return detail::NodeAsPrimitive<std::int32_t>(n_); }
void ClientCapabilities::Union5::set_integer(std::int32_t val) { detail::SetPrimitive(n_, val); }
bool ClientCapabilities::Union5::holds_uinteger() const { return n_.has_child("TODO"); }
std::uint32_t ClientCapabilities::Union5::as_uinteger() { return detail::NodeAsPrimitive<std::uint32_t>(n_); }
void ClientCapabilities::Union5::set_uinteger(std::uint32_t val) { detail::SetPrimitive(n_, val); }
bool ClientCapabilities::Union5::holds_decimal() const { return n_.has_child("TODO"); }
double ClientCapabilities::Union5::as_decimal() { return detail::NodeAsPrimitive<double>(n_); }
void ClientCapabilities::Union5::set_decimal(double val) { detail::SetPrimitive(n_, val); }
bool ClientCapabilities::Union5::holds_boolean() const { return n_.has_child("TODO"); }
bool ClientCapabilities::Union5::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ClientCapabilities::Union5::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
ClientCapabilities::Union5 ClientCapabilities::experimental() { return n_.find_child("experimental"); }
bool ClientCapabilities::has_experimental() const { return n_.has_child("experimental"); }
ClientCapabilities::Union5 ClientCapabilities::add_experimental() { auto w = ClientCapabilities::Union5(n_.append_child() << ryml::key("experimental")); w.init(); return w; }
void ClientCapabilities::init() { StructWrapper::init();  }
bool TextDocumentSyncOptions::openClose() { return detail::NodeAsPrimitive<bool>(n_.find_child("openClose")); }
bool TextDocumentSyncOptions::has_openClose() const { return n_.has_child("openClose"); }
void TextDocumentSyncOptions::set_openClose(bool val) { auto c = n_.find_child("openClose"); if (c.invalid()) { c = n_.append_child() << ryml::key("openClose"); } detail::SetPrimitive(c, val); }
TextDocumentSyncKind TextDocumentSyncOptions::change() { return detail::DeserializeEnum<TextDocumentSyncKind>(n_.find_child("change")); }
bool TextDocumentSyncOptions::has_change() const { return n_.has_child("change"); }
void TextDocumentSyncOptions::set_change(TextDocumentSyncKind val) { auto c = n_.find_child("change"); if (c.invalid()) { c = n_.append_child() << ryml::key("change"); } detail::SerializeEnum(val, c); }
bool TextDocumentSyncOptions::willSave() { return detail::NodeAsPrimitive<bool>(n_.find_child("willSave")); }
bool TextDocumentSyncOptions::has_willSave() const { return n_.has_child("willSave"); }
void TextDocumentSyncOptions::set_willSave(bool val) { auto c = n_.find_child("willSave"); if (c.invalid()) { c = n_.append_child() << ryml::key("willSave"); } detail::SetPrimitive(c, val); }
bool TextDocumentSyncOptions::willSaveWaitUntil() { return detail::NodeAsPrimitive<bool>(n_.find_child("willSaveWaitUntil")); }
bool TextDocumentSyncOptions::has_willSaveWaitUntil() const { return n_.has_child("willSaveWaitUntil"); }
void TextDocumentSyncOptions::set_willSaveWaitUntil(bool val) { auto c = n_.find_child("willSaveWaitUntil"); if (c.invalid()) { c = n_.append_child() << ryml::key("willSaveWaitUntil"); } detail::SetPrimitive(c, val); }
bool TextDocumentSyncOptions::Union4::holds_boolean() const { return n_.has_child("TODO"); }
bool TextDocumentSyncOptions::Union4::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void TextDocumentSyncOptions::Union4::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool TextDocumentSyncOptions::Union4::holds_SaveOptions() const { return n_.has_child("TODO"); }
SaveOptions TextDocumentSyncOptions::Union4::as_SaveOptions() { return {n_}; }
TextDocumentSyncOptions::Union4 TextDocumentSyncOptions::save() { return n_.find_child("save"); }
bool TextDocumentSyncOptions::has_save() const { return n_.has_child("save"); }
TextDocumentSyncOptions::Union4 TextDocumentSyncOptions::add_save() { auto w = TextDocumentSyncOptions::Union4(n_.append_child() << ryml::key("save")); w.init(); return w; }
void TextDocumentSyncOptions::init() { StructWrapper::init();  }
WorkspaceFoldersServerCapabilities WorkspaceOptions::workspaceFolders() { return n_.find_child("workspaceFolders"); }
bool WorkspaceOptions::has_workspaceFolders() const { return n_.has_child("workspaceFolders"); }
WorkspaceFoldersServerCapabilities WorkspaceOptions::add_workspaceFolders() { auto w = WorkspaceFoldersServerCapabilities(n_.append_child() << ryml::key("workspaceFolders")); w.init(); return w; }
FileOperationOptions WorkspaceOptions::fileOperations() { return n_.find_child("fileOperations"); }
bool WorkspaceOptions::has_fileOperations() const { return n_.has_child("fileOperations"); }
FileOperationOptions WorkspaceOptions::add_fileOperations() { auto w = FileOperationOptions(n_.append_child() << ryml::key("fileOperations")); w.init(); return w; }
bool WorkspaceOptions::Union2::holds_TextDocumentContentOptions() const { return n_.has_child("TODO"); }
TextDocumentContentOptions WorkspaceOptions::Union2::as_TextDocumentContentOptions() { return {n_}; }
bool WorkspaceOptions::Union2::holds_TextDocumentContentRegistrationOptions() const { return n_.has_child("TODO"); }
TextDocumentContentRegistrationOptions WorkspaceOptions::Union2::as_TextDocumentContentRegistrationOptions() { return {n_}; }
WorkspaceOptions::Union2 WorkspaceOptions::textDocumentContent() { return n_.find_child("textDocumentContent"); }
bool WorkspaceOptions::has_textDocumentContent() const { return n_.has_child("textDocumentContent"); }
WorkspaceOptions::Union2 WorkspaceOptions::add_textDocumentContent() { auto w = WorkspaceOptions::Union2(n_.append_child() << ryml::key("textDocumentContent")); w.init(); return w; }
void WorkspaceOptions::init() { StructWrapper::init();  }
Range TextDocumentContentChangePartial::range() { return n_.find_child("range"); }
std::uint32_t TextDocumentContentChangePartial::rangeLength() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("rangeLength")); }
bool TextDocumentContentChangePartial::has_rangeLength() const { return n_.has_child("rangeLength"); }
void TextDocumentContentChangePartial::set_rangeLength(std::uint32_t val) { auto c = n_.find_child("rangeLength"); if (c.invalid()) { c = n_.append_child() << ryml::key("rangeLength"); } detail::SetPrimitive(c, val); }
std::string_view TextDocumentContentChangePartial::text() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("text")); }
void TextDocumentContentChangePartial::set_text(std::string_view val) { auto c = n_.find_child("text"); if (c.invalid()) { c = n_.append_child() << ryml::key("text"); } detail::SetPrimitive(c, val); }
void TextDocumentContentChangePartial::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("range")).init(); }
std::string_view TextDocumentContentChangeWholeDocument::text() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("text")); }
void TextDocumentContentChangeWholeDocument::set_text(std::string_view val) { auto c = n_.find_child("text"); if (c.invalid()) { c = n_.append_child() << ryml::key("text"); } detail::SetPrimitive(c, val); }
void TextDocumentContentChangeWholeDocument::init() { StructWrapper::init();  }
std::string_view CodeDescription::href() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("href")); }
void CodeDescription::set_href(std::string_view val) { auto c = n_.find_child("href"); if (c.invalid()) { c = n_.append_child() << ryml::key("href"); } detail::SetPrimitive(c, val); }
void CodeDescription::init() { StructWrapper::init();  }
Location DiagnosticRelatedInformation::location() { return n_.find_child("location"); }
std::string_view DiagnosticRelatedInformation::message() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("message")); }
void DiagnosticRelatedInformation::set_message(std::string_view val) { auto c = n_.find_child("message"); if (c.invalid()) { c = n_.append_child() << ryml::key("message"); } detail::SetPrimitive(c, val); }
void DiagnosticRelatedInformation::init() { StructWrapper::init(); Location(n_.append_child() << ryml::key("location")).init(); }
Range EditRangeWithInsertReplace::insert() { return n_.find_child("insert"); }
Range EditRangeWithInsertReplace::replace() { return n_.find_child("replace"); }
void EditRangeWithInsertReplace::init() { StructWrapper::init(); Range(n_.append_child() << ryml::key("insert")).init(); Range(n_.append_child() << ryml::key("replace")).init(); }
bool ServerCompletionItemOptions::labelDetailsSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("labelDetailsSupport")); }
bool ServerCompletionItemOptions::has_labelDetailsSupport() const { return n_.has_child("labelDetailsSupport"); }
void ServerCompletionItemOptions::set_labelDetailsSupport(bool val) { auto c = n_.find_child("labelDetailsSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("labelDetailsSupport"); } detail::SetPrimitive(c, val); }
void ServerCompletionItemOptions::init() { StructWrapper::init();  }
std::string_view MarkedStringWithLanguage::language() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("language")); }
void MarkedStringWithLanguage::set_language(std::string_view val) { auto c = n_.find_child("language"); if (c.invalid()) { c = n_.append_child() << ryml::key("language"); } detail::SetPrimitive(c, val); }
std::string_view MarkedStringWithLanguage::value() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("value")); }
void MarkedStringWithLanguage::set_value(std::string_view val) { auto c = n_.find_child("value"); if (c.invalid()) { c = n_.append_child() << ryml::key("value"); } detail::SetPrimitive(c, val); }
void MarkedStringWithLanguage::init() { StructWrapper::init();  }
bool ParameterInformation::Union0::holds_string() const { return n_.has_child("TODO"); }
std::string_view ParameterInformation::Union0::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void ParameterInformation::Union0::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
ParameterInformation::Union0 ParameterInformation::label() { return n_.find_child("label"); }
bool ParameterInformation::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view ParameterInformation::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void ParameterInformation::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool ParameterInformation::Union1::holds_MarkupContent() const { return n_.has_child("TODO"); }
MarkupContent ParameterInformation::Union1::as_MarkupContent() { return {n_}; }
ParameterInformation::Union1 ParameterInformation::documentation() { return n_.find_child("documentation"); }
bool ParameterInformation::has_documentation() const { return n_.has_child("documentation"); }
ParameterInformation::Union1 ParameterInformation::add_documentation() { auto w = ParameterInformation::Union1(n_.append_child() << ryml::key("documentation")); w.init(); return w; }
void ParameterInformation::init() { StructWrapper::init(); ParameterInformation::Union0(n_.append_child() << ryml::key("label")).init(); }
CodeActionKind CodeActionKindDocumentation::kind() { return detail::DeserializeEnum<CodeActionKind>(n_.find_child("kind")); }
void CodeActionKindDocumentation::set_kind(CodeActionKind val) { auto c = n_.find_child("kind"); if (c.invalid()) { c = n_.append_child() << ryml::key("kind"); } detail::SerializeEnum(val, c); }
Command CodeActionKindDocumentation::command() { return n_.find_child("command"); }
void CodeActionKindDocumentation::init() { StructWrapper::init(); Command(n_.append_child() << ryml::key("command")).init(); }
bool NotebookCellTextDocumentFilter::Union0::holds_string() const { return n_.has_child("TODO"); }
std::string_view NotebookCellTextDocumentFilter::Union0::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void NotebookCellTextDocumentFilter::Union0::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool NotebookCellTextDocumentFilter::Union0::Union1::holds_NotebookDocumentFilterNotebookType() const { return n_.has_child("TODO"); }
NotebookDocumentFilterNotebookType NotebookCellTextDocumentFilter::Union0::Union1::as_NotebookDocumentFilterNotebookType() { return {n_}; }
bool NotebookCellTextDocumentFilter::Union0::Union1::holds_NotebookDocumentFilterScheme() const { return n_.has_child("TODO"); }
NotebookDocumentFilterScheme NotebookCellTextDocumentFilter::Union0::Union1::as_NotebookDocumentFilterScheme() { return {n_}; }
bool NotebookCellTextDocumentFilter::Union0::Union1::holds_NotebookDocumentFilterPattern() const { return n_.has_child("TODO"); }
NotebookDocumentFilterPattern NotebookCellTextDocumentFilter::Union0::Union1::as_NotebookDocumentFilterPattern() { return {n_}; }
bool NotebookCellTextDocumentFilter::Union0::holds_reference_Union1() const { return n_.has_child("TODO"); }
NotebookDocumentFilter NotebookCellTextDocumentFilter::Union0::as_reference_Union1() {  }
NotebookCellTextDocumentFilter::Union0 NotebookCellTextDocumentFilter::notebook() { return n_.find_child("notebook"); }
std::string_view NotebookCellTextDocumentFilter::language() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("language")); }
bool NotebookCellTextDocumentFilter::has_language() const { return n_.has_child("language"); }
void NotebookCellTextDocumentFilter::set_language(std::string_view val) { auto c = n_.find_child("language"); if (c.invalid()) { c = n_.append_child() << ryml::key("language"); } detail::SetPrimitive(c, val); }
void NotebookCellTextDocumentFilter::init() { StructWrapper::init(); NotebookCellTextDocumentFilter::Union0(n_.append_child() << ryml::key("notebook")).init(); }
bool FileOperationPatternOptions::ignoreCase() { return detail::NodeAsPrimitive<bool>(n_.find_child("ignoreCase")); }
bool FileOperationPatternOptions::has_ignoreCase() const { return n_.has_child("ignoreCase"); }
void FileOperationPatternOptions::set_ignoreCase(bool val) { auto c = n_.find_child("ignoreCase"); if (c.invalid()) { c = n_.append_child() << ryml::key("ignoreCase"); } detail::SetPrimitive(c, val); }
void FileOperationPatternOptions::init() { StructWrapper::init();  }
std::uint32_t ExecutionSummary::executionOrder() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("executionOrder")); }
void ExecutionSummary::set_executionOrder(std::uint32_t val) { auto c = n_.find_child("executionOrder"); if (c.invalid()) { c = n_.append_child() << ryml::key("executionOrder"); } detail::SetPrimitive(c, val); }
bool ExecutionSummary::success() { return detail::NodeAsPrimitive<bool>(n_.find_child("success")); }
bool ExecutionSummary::has_success() const { return n_.has_child("success"); }
void ExecutionSummary::set_success(bool val) { auto c = n_.find_child("success"); if (c.invalid()) { c = n_.append_child() << ryml::key("success"); } detail::SetPrimitive(c, val); }
void ExecutionSummary::init() { StructWrapper::init();  }
std::string_view NotebookCellLanguage::language() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("language")); }
void NotebookCellLanguage::set_language(std::string_view val) { auto c = n_.find_child("language"); if (c.invalid()) { c = n_.append_child() << ryml::key("language"); } detail::SetPrimitive(c, val); }
void NotebookCellLanguage::init() { StructWrapper::init();  }
NotebookCellArrayChange NotebookDocumentCellChangeStructure::array() { return n_.find_child("array"); }
List<TextDocumentItem> NotebookDocumentCellChangeStructure::didOpen() { return n_.find_child("didOpen"); }
bool NotebookDocumentCellChangeStructure::has_didOpen() const { return n_.has_child("didOpen"); }
List<TextDocumentItem> NotebookDocumentCellChangeStructure::add_didOpen() { auto w = List<TextDocumentItem>(n_.append_child() << ryml::key("didOpen")); w.init(); return w; }
List<TextDocumentIdentifier> NotebookDocumentCellChangeStructure::didClose() { return n_.find_child("didClose"); }
bool NotebookDocumentCellChangeStructure::has_didClose() const { return n_.has_child("didClose"); }
List<TextDocumentIdentifier> NotebookDocumentCellChangeStructure::add_didClose() { auto w = List<TextDocumentIdentifier>(n_.append_child() << ryml::key("didClose")); w.init(); return w; }
void NotebookDocumentCellChangeStructure::init() { StructWrapper::init(); NotebookCellArrayChange(n_.append_child() << ryml::key("array")).init(); }
VersionedTextDocumentIdentifier NotebookDocumentCellContentChanges::document() { return n_.find_child("document"); }
List<TextDocumentContentChangeEvent> NotebookDocumentCellContentChanges::changes() { return n_.find_child("changes"); }
void NotebookDocumentCellContentChanges::init() { StructWrapper::init(); VersionedTextDocumentIdentifier(n_.append_child() << ryml::key("document")).init(); List<TextDocumentContentChangeEvent>(n_.append_child() << ryml::key("changes")).init(); }
bool WorkspaceClientCapabilities::applyEdit() { return detail::NodeAsPrimitive<bool>(n_.find_child("applyEdit")); }
bool WorkspaceClientCapabilities::has_applyEdit() const { return n_.has_child("applyEdit"); }
void WorkspaceClientCapabilities::set_applyEdit(bool val) { auto c = n_.find_child("applyEdit"); if (c.invalid()) { c = n_.append_child() << ryml::key("applyEdit"); } detail::SetPrimitive(c, val); }
WorkspaceEditClientCapabilities WorkspaceClientCapabilities::workspaceEdit() { return n_.find_child("workspaceEdit"); }
bool WorkspaceClientCapabilities::has_workspaceEdit() const { return n_.has_child("workspaceEdit"); }
WorkspaceEditClientCapabilities WorkspaceClientCapabilities::add_workspaceEdit() { auto w = WorkspaceEditClientCapabilities(n_.append_child() << ryml::key("workspaceEdit")); w.init(); return w; }
DidChangeConfigurationClientCapabilities WorkspaceClientCapabilities::didChangeConfiguration() { return n_.find_child("didChangeConfiguration"); }
bool WorkspaceClientCapabilities::has_didChangeConfiguration() const { return n_.has_child("didChangeConfiguration"); }
DidChangeConfigurationClientCapabilities WorkspaceClientCapabilities::add_didChangeConfiguration() { auto w = DidChangeConfigurationClientCapabilities(n_.append_child() << ryml::key("didChangeConfiguration")); w.init(); return w; }
DidChangeWatchedFilesClientCapabilities WorkspaceClientCapabilities::didChangeWatchedFiles() { return n_.find_child("didChangeWatchedFiles"); }
bool WorkspaceClientCapabilities::has_didChangeWatchedFiles() const { return n_.has_child("didChangeWatchedFiles"); }
DidChangeWatchedFilesClientCapabilities WorkspaceClientCapabilities::add_didChangeWatchedFiles() { auto w = DidChangeWatchedFilesClientCapabilities(n_.append_child() << ryml::key("didChangeWatchedFiles")); w.init(); return w; }
WorkspaceSymbolClientCapabilities WorkspaceClientCapabilities::symbol() { return n_.find_child("symbol"); }
bool WorkspaceClientCapabilities::has_symbol() const { return n_.has_child("symbol"); }
WorkspaceSymbolClientCapabilities WorkspaceClientCapabilities::add_symbol() { auto w = WorkspaceSymbolClientCapabilities(n_.append_child() << ryml::key("symbol")); w.init(); return w; }
ExecuteCommandClientCapabilities WorkspaceClientCapabilities::executeCommand() { return n_.find_child("executeCommand"); }
bool WorkspaceClientCapabilities::has_executeCommand() const { return n_.has_child("executeCommand"); }
ExecuteCommandClientCapabilities WorkspaceClientCapabilities::add_executeCommand() { auto w = ExecuteCommandClientCapabilities(n_.append_child() << ryml::key("executeCommand")); w.init(); return w; }
bool WorkspaceClientCapabilities::workspaceFolders() { return detail::NodeAsPrimitive<bool>(n_.find_child("workspaceFolders")); }
bool WorkspaceClientCapabilities::has_workspaceFolders() const { return n_.has_child("workspaceFolders"); }
void WorkspaceClientCapabilities::set_workspaceFolders(bool val) { auto c = n_.find_child("workspaceFolders"); if (c.invalid()) { c = n_.append_child() << ryml::key("workspaceFolders"); } detail::SetPrimitive(c, val); }
bool WorkspaceClientCapabilities::configuration() { return detail::NodeAsPrimitive<bool>(n_.find_child("configuration")); }
bool WorkspaceClientCapabilities::has_configuration() const { return n_.has_child("configuration"); }
void WorkspaceClientCapabilities::set_configuration(bool val) { auto c = n_.find_child("configuration"); if (c.invalid()) { c = n_.append_child() << ryml::key("configuration"); } detail::SetPrimitive(c, val); }
SemanticTokensWorkspaceClientCapabilities WorkspaceClientCapabilities::semanticTokens() { return n_.find_child("semanticTokens"); }
bool WorkspaceClientCapabilities::has_semanticTokens() const { return n_.has_child("semanticTokens"); }
SemanticTokensWorkspaceClientCapabilities WorkspaceClientCapabilities::add_semanticTokens() { auto w = SemanticTokensWorkspaceClientCapabilities(n_.append_child() << ryml::key("semanticTokens")); w.init(); return w; }
CodeLensWorkspaceClientCapabilities WorkspaceClientCapabilities::codeLens() { return n_.find_child("codeLens"); }
bool WorkspaceClientCapabilities::has_codeLens() const { return n_.has_child("codeLens"); }
CodeLensWorkspaceClientCapabilities WorkspaceClientCapabilities::add_codeLens() { auto w = CodeLensWorkspaceClientCapabilities(n_.append_child() << ryml::key("codeLens")); w.init(); return w; }
FileOperationClientCapabilities WorkspaceClientCapabilities::fileOperations() { return n_.find_child("fileOperations"); }
bool WorkspaceClientCapabilities::has_fileOperations() const { return n_.has_child("fileOperations"); }
FileOperationClientCapabilities WorkspaceClientCapabilities::add_fileOperations() { auto w = FileOperationClientCapabilities(n_.append_child() << ryml::key("fileOperations")); w.init(); return w; }
InlineValueWorkspaceClientCapabilities WorkspaceClientCapabilities::inlineValue() { return n_.find_child("inlineValue"); }
bool WorkspaceClientCapabilities::has_inlineValue() const { return n_.has_child("inlineValue"); }
InlineValueWorkspaceClientCapabilities WorkspaceClientCapabilities::add_inlineValue() { auto w = InlineValueWorkspaceClientCapabilities(n_.append_child() << ryml::key("inlineValue")); w.init(); return w; }
InlayHintWorkspaceClientCapabilities WorkspaceClientCapabilities::inlayHint() { return n_.find_child("inlayHint"); }
bool WorkspaceClientCapabilities::has_inlayHint() const { return n_.has_child("inlayHint"); }
InlayHintWorkspaceClientCapabilities WorkspaceClientCapabilities::add_inlayHint() { auto w = InlayHintWorkspaceClientCapabilities(n_.append_child() << ryml::key("inlayHint")); w.init(); return w; }
DiagnosticWorkspaceClientCapabilities WorkspaceClientCapabilities::diagnostics() { return n_.find_child("diagnostics"); }
bool WorkspaceClientCapabilities::has_diagnostics() const { return n_.has_child("diagnostics"); }
DiagnosticWorkspaceClientCapabilities WorkspaceClientCapabilities::add_diagnostics() { auto w = DiagnosticWorkspaceClientCapabilities(n_.append_child() << ryml::key("diagnostics")); w.init(); return w; }
FoldingRangeWorkspaceClientCapabilities WorkspaceClientCapabilities::foldingRange() { return n_.find_child("foldingRange"); }
bool WorkspaceClientCapabilities::has_foldingRange() const { return n_.has_child("foldingRange"); }
FoldingRangeWorkspaceClientCapabilities WorkspaceClientCapabilities::add_foldingRange() { auto w = FoldingRangeWorkspaceClientCapabilities(n_.append_child() << ryml::key("foldingRange")); w.init(); return w; }
TextDocumentContentClientCapabilities WorkspaceClientCapabilities::textDocumentContent() { return n_.find_child("textDocumentContent"); }
bool WorkspaceClientCapabilities::has_textDocumentContent() const { return n_.has_child("textDocumentContent"); }
TextDocumentContentClientCapabilities WorkspaceClientCapabilities::add_textDocumentContent() { auto w = TextDocumentContentClientCapabilities(n_.append_child() << ryml::key("textDocumentContent")); w.init(); return w; }
void WorkspaceClientCapabilities::init() { StructWrapper::init();  }
TextDocumentSyncClientCapabilities TextDocumentClientCapabilities::synchronization() { return n_.find_child("synchronization"); }
bool TextDocumentClientCapabilities::has_synchronization() const { return n_.has_child("synchronization"); }
TextDocumentSyncClientCapabilities TextDocumentClientCapabilities::add_synchronization() { auto w = TextDocumentSyncClientCapabilities(n_.append_child() << ryml::key("synchronization")); w.init(); return w; }
TextDocumentFilterClientCapabilities TextDocumentClientCapabilities::filters() { return n_.find_child("filters"); }
bool TextDocumentClientCapabilities::has_filters() const { return n_.has_child("filters"); }
TextDocumentFilterClientCapabilities TextDocumentClientCapabilities::add_filters() { auto w = TextDocumentFilterClientCapabilities(n_.append_child() << ryml::key("filters")); w.init(); return w; }
CompletionClientCapabilities TextDocumentClientCapabilities::completion() { return n_.find_child("completion"); }
bool TextDocumentClientCapabilities::has_completion() const { return n_.has_child("completion"); }
CompletionClientCapabilities TextDocumentClientCapabilities::add_completion() { auto w = CompletionClientCapabilities(n_.append_child() << ryml::key("completion")); w.init(); return w; }
HoverClientCapabilities TextDocumentClientCapabilities::hover() { return n_.find_child("hover"); }
bool TextDocumentClientCapabilities::has_hover() const { return n_.has_child("hover"); }
HoverClientCapabilities TextDocumentClientCapabilities::add_hover() { auto w = HoverClientCapabilities(n_.append_child() << ryml::key("hover")); w.init(); return w; }
SignatureHelpClientCapabilities TextDocumentClientCapabilities::signatureHelp() { return n_.find_child("signatureHelp"); }
bool TextDocumentClientCapabilities::has_signatureHelp() const { return n_.has_child("signatureHelp"); }
SignatureHelpClientCapabilities TextDocumentClientCapabilities::add_signatureHelp() { auto w = SignatureHelpClientCapabilities(n_.append_child() << ryml::key("signatureHelp")); w.init(); return w; }
DeclarationClientCapabilities TextDocumentClientCapabilities::declaration() { return n_.find_child("declaration"); }
bool TextDocumentClientCapabilities::has_declaration() const { return n_.has_child("declaration"); }
DeclarationClientCapabilities TextDocumentClientCapabilities::add_declaration() { auto w = DeclarationClientCapabilities(n_.append_child() << ryml::key("declaration")); w.init(); return w; }
DefinitionClientCapabilities TextDocumentClientCapabilities::definition() { return n_.find_child("definition"); }
bool TextDocumentClientCapabilities::has_definition() const { return n_.has_child("definition"); }
DefinitionClientCapabilities TextDocumentClientCapabilities::add_definition() { auto w = DefinitionClientCapabilities(n_.append_child() << ryml::key("definition")); w.init(); return w; }
TypeDefinitionClientCapabilities TextDocumentClientCapabilities::typeDefinition() { return n_.find_child("typeDefinition"); }
bool TextDocumentClientCapabilities::has_typeDefinition() const { return n_.has_child("typeDefinition"); }
TypeDefinitionClientCapabilities TextDocumentClientCapabilities::add_typeDefinition() { auto w = TypeDefinitionClientCapabilities(n_.append_child() << ryml::key("typeDefinition")); w.init(); return w; }
ImplementationClientCapabilities TextDocumentClientCapabilities::implementation() { return n_.find_child("implementation"); }
bool TextDocumentClientCapabilities::has_implementation() const { return n_.has_child("implementation"); }
ImplementationClientCapabilities TextDocumentClientCapabilities::add_implementation() { auto w = ImplementationClientCapabilities(n_.append_child() << ryml::key("implementation")); w.init(); return w; }
ReferenceClientCapabilities TextDocumentClientCapabilities::references() { return n_.find_child("references"); }
bool TextDocumentClientCapabilities::has_references() const { return n_.has_child("references"); }
ReferenceClientCapabilities TextDocumentClientCapabilities::add_references() { auto w = ReferenceClientCapabilities(n_.append_child() << ryml::key("references")); w.init(); return w; }
DocumentHighlightClientCapabilities TextDocumentClientCapabilities::documentHighlight() { return n_.find_child("documentHighlight"); }
bool TextDocumentClientCapabilities::has_documentHighlight() const { return n_.has_child("documentHighlight"); }
DocumentHighlightClientCapabilities TextDocumentClientCapabilities::add_documentHighlight() { auto w = DocumentHighlightClientCapabilities(n_.append_child() << ryml::key("documentHighlight")); w.init(); return w; }
DocumentSymbolClientCapabilities TextDocumentClientCapabilities::documentSymbol() { return n_.find_child("documentSymbol"); }
bool TextDocumentClientCapabilities::has_documentSymbol() const { return n_.has_child("documentSymbol"); }
DocumentSymbolClientCapabilities TextDocumentClientCapabilities::add_documentSymbol() { auto w = DocumentSymbolClientCapabilities(n_.append_child() << ryml::key("documentSymbol")); w.init(); return w; }
CodeActionClientCapabilities TextDocumentClientCapabilities::codeAction() { return n_.find_child("codeAction"); }
bool TextDocumentClientCapabilities::has_codeAction() const { return n_.has_child("codeAction"); }
CodeActionClientCapabilities TextDocumentClientCapabilities::add_codeAction() { auto w = CodeActionClientCapabilities(n_.append_child() << ryml::key("codeAction")); w.init(); return w; }
CodeLensClientCapabilities TextDocumentClientCapabilities::codeLens() { return n_.find_child("codeLens"); }
bool TextDocumentClientCapabilities::has_codeLens() const { return n_.has_child("codeLens"); }
CodeLensClientCapabilities TextDocumentClientCapabilities::add_codeLens() { auto w = CodeLensClientCapabilities(n_.append_child() << ryml::key("codeLens")); w.init(); return w; }
DocumentLinkClientCapabilities TextDocumentClientCapabilities::documentLink() { return n_.find_child("documentLink"); }
bool TextDocumentClientCapabilities::has_documentLink() const { return n_.has_child("documentLink"); }
DocumentLinkClientCapabilities TextDocumentClientCapabilities::add_documentLink() { auto w = DocumentLinkClientCapabilities(n_.append_child() << ryml::key("documentLink")); w.init(); return w; }
DocumentColorClientCapabilities TextDocumentClientCapabilities::colorProvider() { return n_.find_child("colorProvider"); }
bool TextDocumentClientCapabilities::has_colorProvider() const { return n_.has_child("colorProvider"); }
DocumentColorClientCapabilities TextDocumentClientCapabilities::add_colorProvider() { auto w = DocumentColorClientCapabilities(n_.append_child() << ryml::key("colorProvider")); w.init(); return w; }
DocumentFormattingClientCapabilities TextDocumentClientCapabilities::formatting() { return n_.find_child("formatting"); }
bool TextDocumentClientCapabilities::has_formatting() const { return n_.has_child("formatting"); }
DocumentFormattingClientCapabilities TextDocumentClientCapabilities::add_formatting() { auto w = DocumentFormattingClientCapabilities(n_.append_child() << ryml::key("formatting")); w.init(); return w; }
DocumentRangeFormattingClientCapabilities TextDocumentClientCapabilities::rangeFormatting() { return n_.find_child("rangeFormatting"); }
bool TextDocumentClientCapabilities::has_rangeFormatting() const { return n_.has_child("rangeFormatting"); }
DocumentRangeFormattingClientCapabilities TextDocumentClientCapabilities::add_rangeFormatting() { auto w = DocumentRangeFormattingClientCapabilities(n_.append_child() << ryml::key("rangeFormatting")); w.init(); return w; }
DocumentOnTypeFormattingClientCapabilities TextDocumentClientCapabilities::onTypeFormatting() { return n_.find_child("onTypeFormatting"); }
bool TextDocumentClientCapabilities::has_onTypeFormatting() const { return n_.has_child("onTypeFormatting"); }
DocumentOnTypeFormattingClientCapabilities TextDocumentClientCapabilities::add_onTypeFormatting() { auto w = DocumentOnTypeFormattingClientCapabilities(n_.append_child() << ryml::key("onTypeFormatting")); w.init(); return w; }
RenameClientCapabilities TextDocumentClientCapabilities::rename() { return n_.find_child("rename"); }
bool TextDocumentClientCapabilities::has_rename() const { return n_.has_child("rename"); }
RenameClientCapabilities TextDocumentClientCapabilities::add_rename() { auto w = RenameClientCapabilities(n_.append_child() << ryml::key("rename")); w.init(); return w; }
FoldingRangeClientCapabilities TextDocumentClientCapabilities::foldingRange() { return n_.find_child("foldingRange"); }
bool TextDocumentClientCapabilities::has_foldingRange() const { return n_.has_child("foldingRange"); }
FoldingRangeClientCapabilities TextDocumentClientCapabilities::add_foldingRange() { auto w = FoldingRangeClientCapabilities(n_.append_child() << ryml::key("foldingRange")); w.init(); return w; }
SelectionRangeClientCapabilities TextDocumentClientCapabilities::selectionRange() { return n_.find_child("selectionRange"); }
bool TextDocumentClientCapabilities::has_selectionRange() const { return n_.has_child("selectionRange"); }
SelectionRangeClientCapabilities TextDocumentClientCapabilities::add_selectionRange() { auto w = SelectionRangeClientCapabilities(n_.append_child() << ryml::key("selectionRange")); w.init(); return w; }
PublishDiagnosticsClientCapabilities TextDocumentClientCapabilities::publishDiagnostics() { return n_.find_child("publishDiagnostics"); }
bool TextDocumentClientCapabilities::has_publishDiagnostics() const { return n_.has_child("publishDiagnostics"); }
PublishDiagnosticsClientCapabilities TextDocumentClientCapabilities::add_publishDiagnostics() { auto w = PublishDiagnosticsClientCapabilities(n_.append_child() << ryml::key("publishDiagnostics")); w.init(); return w; }
CallHierarchyClientCapabilities TextDocumentClientCapabilities::callHierarchy() { return n_.find_child("callHierarchy"); }
bool TextDocumentClientCapabilities::has_callHierarchy() const { return n_.has_child("callHierarchy"); }
CallHierarchyClientCapabilities TextDocumentClientCapabilities::add_callHierarchy() { auto w = CallHierarchyClientCapabilities(n_.append_child() << ryml::key("callHierarchy")); w.init(); return w; }
SemanticTokensClientCapabilities TextDocumentClientCapabilities::semanticTokens() { return n_.find_child("semanticTokens"); }
bool TextDocumentClientCapabilities::has_semanticTokens() const { return n_.has_child("semanticTokens"); }
SemanticTokensClientCapabilities TextDocumentClientCapabilities::add_semanticTokens() { auto w = SemanticTokensClientCapabilities(n_.append_child() << ryml::key("semanticTokens")); w.init(); return w; }
LinkedEditingRangeClientCapabilities TextDocumentClientCapabilities::linkedEditingRange() { return n_.find_child("linkedEditingRange"); }
bool TextDocumentClientCapabilities::has_linkedEditingRange() const { return n_.has_child("linkedEditingRange"); }
LinkedEditingRangeClientCapabilities TextDocumentClientCapabilities::add_linkedEditingRange() { auto w = LinkedEditingRangeClientCapabilities(n_.append_child() << ryml::key("linkedEditingRange")); w.init(); return w; }
MonikerClientCapabilities TextDocumentClientCapabilities::moniker() { return n_.find_child("moniker"); }
bool TextDocumentClientCapabilities::has_moniker() const { return n_.has_child("moniker"); }
MonikerClientCapabilities TextDocumentClientCapabilities::add_moniker() { auto w = MonikerClientCapabilities(n_.append_child() << ryml::key("moniker")); w.init(); return w; }
TypeHierarchyClientCapabilities TextDocumentClientCapabilities::typeHierarchy() { return n_.find_child("typeHierarchy"); }
bool TextDocumentClientCapabilities::has_typeHierarchy() const { return n_.has_child("typeHierarchy"); }
TypeHierarchyClientCapabilities TextDocumentClientCapabilities::add_typeHierarchy() { auto w = TypeHierarchyClientCapabilities(n_.append_child() << ryml::key("typeHierarchy")); w.init(); return w; }
InlineValueClientCapabilities TextDocumentClientCapabilities::inlineValue() { return n_.find_child("inlineValue"); }
bool TextDocumentClientCapabilities::has_inlineValue() const { return n_.has_child("inlineValue"); }
InlineValueClientCapabilities TextDocumentClientCapabilities::add_inlineValue() { auto w = InlineValueClientCapabilities(n_.append_child() << ryml::key("inlineValue")); w.init(); return w; }
InlayHintClientCapabilities TextDocumentClientCapabilities::inlayHint() { return n_.find_child("inlayHint"); }
bool TextDocumentClientCapabilities::has_inlayHint() const { return n_.has_child("inlayHint"); }
InlayHintClientCapabilities TextDocumentClientCapabilities::add_inlayHint() { auto w = InlayHintClientCapabilities(n_.append_child() << ryml::key("inlayHint")); w.init(); return w; }
DiagnosticClientCapabilities TextDocumentClientCapabilities::diagnostic() { return n_.find_child("diagnostic"); }
bool TextDocumentClientCapabilities::has_diagnostic() const { return n_.has_child("diagnostic"); }
DiagnosticClientCapabilities TextDocumentClientCapabilities::add_diagnostic() { auto w = DiagnosticClientCapabilities(n_.append_child() << ryml::key("diagnostic")); w.init(); return w; }
InlineCompletionClientCapabilities TextDocumentClientCapabilities::inlineCompletion() { return n_.find_child("inlineCompletion"); }
bool TextDocumentClientCapabilities::has_inlineCompletion() const { return n_.has_child("inlineCompletion"); }
InlineCompletionClientCapabilities TextDocumentClientCapabilities::add_inlineCompletion() { auto w = InlineCompletionClientCapabilities(n_.append_child() << ryml::key("inlineCompletion")); w.init(); return w; }
void TextDocumentClientCapabilities::init() { StructWrapper::init();  }
NotebookDocumentSyncClientCapabilities NotebookDocumentClientCapabilities::synchronization() { return n_.find_child("synchronization"); }
void NotebookDocumentClientCapabilities::init() { StructWrapper::init(); NotebookDocumentSyncClientCapabilities(n_.append_child() << ryml::key("synchronization")).init(); }
bool WindowClientCapabilities::workDoneProgress() { return detail::NodeAsPrimitive<bool>(n_.find_child("workDoneProgress")); }
bool WindowClientCapabilities::has_workDoneProgress() const { return n_.has_child("workDoneProgress"); }
void WindowClientCapabilities::set_workDoneProgress(bool val) { auto c = n_.find_child("workDoneProgress"); if (c.invalid()) { c = n_.append_child() << ryml::key("workDoneProgress"); } detail::SetPrimitive(c, val); }
ShowMessageRequestClientCapabilities WindowClientCapabilities::showMessage() { return n_.find_child("showMessage"); }
bool WindowClientCapabilities::has_showMessage() const { return n_.has_child("showMessage"); }
ShowMessageRequestClientCapabilities WindowClientCapabilities::add_showMessage() { auto w = ShowMessageRequestClientCapabilities(n_.append_child() << ryml::key("showMessage")); w.init(); return w; }
ShowDocumentClientCapabilities WindowClientCapabilities::showDocument() { return n_.find_child("showDocument"); }
bool WindowClientCapabilities::has_showDocument() const { return n_.has_child("showDocument"); }
ShowDocumentClientCapabilities WindowClientCapabilities::add_showDocument() { auto w = ShowDocumentClientCapabilities(n_.append_child() << ryml::key("showDocument")); w.init(); return w; }
void WindowClientCapabilities::init() { StructWrapper::init();  }
StaleRequestSupportOptions GeneralClientCapabilities::staleRequestSupport() { return n_.find_child("staleRequestSupport"); }
bool GeneralClientCapabilities::has_staleRequestSupport() const { return n_.has_child("staleRequestSupport"); }
StaleRequestSupportOptions GeneralClientCapabilities::add_staleRequestSupport() { auto w = StaleRequestSupportOptions(n_.append_child() << ryml::key("staleRequestSupport")); w.init(); return w; }
RegularExpressionsClientCapabilities GeneralClientCapabilities::regularExpressions() { return n_.find_child("regularExpressions"); }
bool GeneralClientCapabilities::has_regularExpressions() const { return n_.has_child("regularExpressions"); }
RegularExpressionsClientCapabilities GeneralClientCapabilities::add_regularExpressions() { auto w = RegularExpressionsClientCapabilities(n_.append_child() << ryml::key("regularExpressions")); w.init(); return w; }
MarkdownClientCapabilities GeneralClientCapabilities::markdown() { return n_.find_child("markdown"); }
bool GeneralClientCapabilities::has_markdown() const { return n_.has_child("markdown"); }
MarkdownClientCapabilities GeneralClientCapabilities::add_markdown() { auto w = MarkdownClientCapabilities(n_.append_child() << ryml::key("markdown")); w.init(); return w; }
List<PositionEncodingKind> GeneralClientCapabilities::positionEncodings() { return n_.find_child("positionEncodings"); }
bool GeneralClientCapabilities::has_positionEncodings() const { return n_.has_child("positionEncodings"); }
List<PositionEncodingKind> GeneralClientCapabilities::add_positionEncodings() { auto w = List<PositionEncodingKind>(n_.append_child() << ryml::key("positionEncodings")); w.init(); return w; }
void GeneralClientCapabilities::init() { StructWrapper::init();  }
bool WorkspaceFoldersServerCapabilities::supported() { return detail::NodeAsPrimitive<bool>(n_.find_child("supported")); }
bool WorkspaceFoldersServerCapabilities::has_supported() const { return n_.has_child("supported"); }
void WorkspaceFoldersServerCapabilities::set_supported(bool val) { auto c = n_.find_child("supported"); if (c.invalid()) { c = n_.append_child() << ryml::key("supported"); } detail::SetPrimitive(c, val); }
bool WorkspaceFoldersServerCapabilities::Union1::holds_string() const { return n_.has_child("TODO"); }
std::string_view WorkspaceFoldersServerCapabilities::Union1::as_string() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void WorkspaceFoldersServerCapabilities::Union1::set_string(std::string_view val) { detail::SetPrimitive(n_, val); }
bool WorkspaceFoldersServerCapabilities::Union1::holds_boolean() const { return n_.has_child("TODO"); }
bool WorkspaceFoldersServerCapabilities::Union1::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void WorkspaceFoldersServerCapabilities::Union1::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
WorkspaceFoldersServerCapabilities::Union1 WorkspaceFoldersServerCapabilities::changeNotifications() { return n_.find_child("changeNotifications"); }
bool WorkspaceFoldersServerCapabilities::has_changeNotifications() const { return n_.has_child("changeNotifications"); }
WorkspaceFoldersServerCapabilities::Union1 WorkspaceFoldersServerCapabilities::add_changeNotifications() { auto w = WorkspaceFoldersServerCapabilities::Union1(n_.append_child() << ryml::key("changeNotifications")); w.init(); return w; }
void WorkspaceFoldersServerCapabilities::init() { StructWrapper::init();  }
FileOperationRegistrationOptions FileOperationOptions::didCreate() { return n_.find_child("didCreate"); }
bool FileOperationOptions::has_didCreate() const { return n_.has_child("didCreate"); }
FileOperationRegistrationOptions FileOperationOptions::add_didCreate() { auto w = FileOperationRegistrationOptions(n_.append_child() << ryml::key("didCreate")); w.init(); return w; }
FileOperationRegistrationOptions FileOperationOptions::willCreate() { return n_.find_child("willCreate"); }
bool FileOperationOptions::has_willCreate() const { return n_.has_child("willCreate"); }
FileOperationRegistrationOptions FileOperationOptions::add_willCreate() { auto w = FileOperationRegistrationOptions(n_.append_child() << ryml::key("willCreate")); w.init(); return w; }
FileOperationRegistrationOptions FileOperationOptions::didRename() { return n_.find_child("didRename"); }
bool FileOperationOptions::has_didRename() const { return n_.has_child("didRename"); }
FileOperationRegistrationOptions FileOperationOptions::add_didRename() { auto w = FileOperationRegistrationOptions(n_.append_child() << ryml::key("didRename")); w.init(); return w; }
FileOperationRegistrationOptions FileOperationOptions::willRename() { return n_.find_child("willRename"); }
bool FileOperationOptions::has_willRename() const { return n_.has_child("willRename"); }
FileOperationRegistrationOptions FileOperationOptions::add_willRename() { auto w = FileOperationRegistrationOptions(n_.append_child() << ryml::key("willRename")); w.init(); return w; }
FileOperationRegistrationOptions FileOperationOptions::didDelete() { return n_.find_child("didDelete"); }
bool FileOperationOptions::has_didDelete() const { return n_.has_child("didDelete"); }
FileOperationRegistrationOptions FileOperationOptions::add_didDelete() { auto w = FileOperationRegistrationOptions(n_.append_child() << ryml::key("didDelete")); w.init(); return w; }
FileOperationRegistrationOptions FileOperationOptions::willDelete() { return n_.find_child("willDelete"); }
bool FileOperationOptions::has_willDelete() const { return n_.has_child("willDelete"); }
FileOperationRegistrationOptions FileOperationOptions::add_willDelete() { auto w = FileOperationRegistrationOptions(n_.append_child() << ryml::key("willDelete")); w.init(); return w; }
void FileOperationOptions::init() { StructWrapper::init();  }
bool RelativePattern::Union0::holds_WorkspaceFolder() const { return n_.has_child("TODO"); }
WorkspaceFolder RelativePattern::Union0::as_WorkspaceFolder() { return {n_}; }
bool RelativePattern::Union0::holds_URI() const { return n_.has_child("TODO"); }
std::string_view RelativePattern::Union0::as_URI() { return detail::NodeAsPrimitive<std::string_view>(n_); }
void RelativePattern::Union0::set_URI(std::string_view val) { detail::SetPrimitive(n_, val); }
RelativePattern::Union0 RelativePattern::baseUri() { return n_.find_child("baseUri"); }
Pattern RelativePattern::pattern() { return detail::NodeAsPrimitive<Pattern>(n_.find_child("pattern")); }
void RelativePattern::set_pattern(Pattern val) { auto c = n_.find_child("pattern"); if (c.invalid()) { c = n_.append_child() << ryml::key("pattern"); } detail::SetPrimitive(c, val); }
void RelativePattern::init() { StructWrapper::init(); RelativePattern::Union0(n_.append_child() << ryml::key("baseUri")).init(); }
std::string_view TextDocumentFilterLanguage::language() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("language")); }
void TextDocumentFilterLanguage::set_language(std::string_view val) { auto c = n_.find_child("language"); if (c.invalid()) { c = n_.append_child() << ryml::key("language"); } detail::SetPrimitive(c, val); }
std::string_view TextDocumentFilterLanguage::scheme() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("scheme")); }
bool TextDocumentFilterLanguage::has_scheme() const { return n_.has_child("scheme"); }
void TextDocumentFilterLanguage::set_scheme(std::string_view val) { auto c = n_.find_child("scheme"); if (c.invalid()) { c = n_.append_child() << ryml::key("scheme"); } detail::SetPrimitive(c, val); }
bool TextDocumentFilterLanguage::Union2::holds_string() const { return n_.has_child("TODO"); }
Pattern TextDocumentFilterLanguage::Union2::as_string() { return detail::NodeAsPrimitive<Pattern>(n_); }
void TextDocumentFilterLanguage::Union2::set_string(Pattern val) { detail::SetPrimitive(n_, val); }
bool TextDocumentFilterLanguage::Union2::holds_RelativePattern() const { return n_.has_child("TODO"); }
RelativePattern TextDocumentFilterLanguage::Union2::as_RelativePattern() { return {n_}; }
TextDocumentFilterLanguage::Union2 TextDocumentFilterLanguage::pattern() { return n_.find_child("pattern"); }
bool TextDocumentFilterLanguage::has_pattern() const { return n_.has_child("pattern"); }
TextDocumentFilterLanguage::Union2 TextDocumentFilterLanguage::add_pattern() { auto w = TextDocumentFilterLanguage::Union2(n_.append_child() << ryml::key("pattern")); w.init(); return w; }
void TextDocumentFilterLanguage::init() { StructWrapper::init();  }
std::string_view TextDocumentFilterScheme::language() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("language")); }
bool TextDocumentFilterScheme::has_language() const { return n_.has_child("language"); }
void TextDocumentFilterScheme::set_language(std::string_view val) { auto c = n_.find_child("language"); if (c.invalid()) { c = n_.append_child() << ryml::key("language"); } detail::SetPrimitive(c, val); }
std::string_view TextDocumentFilterScheme::scheme() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("scheme")); }
void TextDocumentFilterScheme::set_scheme(std::string_view val) { auto c = n_.find_child("scheme"); if (c.invalid()) { c = n_.append_child() << ryml::key("scheme"); } detail::SetPrimitive(c, val); }
bool TextDocumentFilterScheme::Union2::holds_string() const { return n_.has_child("TODO"); }
Pattern TextDocumentFilterScheme::Union2::as_string() { return detail::NodeAsPrimitive<Pattern>(n_); }
void TextDocumentFilterScheme::Union2::set_string(Pattern val) { detail::SetPrimitive(n_, val); }
bool TextDocumentFilterScheme::Union2::holds_RelativePattern() const { return n_.has_child("TODO"); }
RelativePattern TextDocumentFilterScheme::Union2::as_RelativePattern() { return {n_}; }
TextDocumentFilterScheme::Union2 TextDocumentFilterScheme::pattern() { return n_.find_child("pattern"); }
bool TextDocumentFilterScheme::has_pattern() const { return n_.has_child("pattern"); }
TextDocumentFilterScheme::Union2 TextDocumentFilterScheme::add_pattern() { auto w = TextDocumentFilterScheme::Union2(n_.append_child() << ryml::key("pattern")); w.init(); return w; }
void TextDocumentFilterScheme::init() { StructWrapper::init();  }
std::string_view TextDocumentFilterPattern::language() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("language")); }
bool TextDocumentFilterPattern::has_language() const { return n_.has_child("language"); }
void TextDocumentFilterPattern::set_language(std::string_view val) { auto c = n_.find_child("language"); if (c.invalid()) { c = n_.append_child() << ryml::key("language"); } detail::SetPrimitive(c, val); }
std::string_view TextDocumentFilterPattern::scheme() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("scheme")); }
bool TextDocumentFilterPattern::has_scheme() const { return n_.has_child("scheme"); }
void TextDocumentFilterPattern::set_scheme(std::string_view val) { auto c = n_.find_child("scheme"); if (c.invalid()) { c = n_.append_child() << ryml::key("scheme"); } detail::SetPrimitive(c, val); }
bool TextDocumentFilterPattern::Union2::holds_string() const { return n_.has_child("TODO"); }
Pattern TextDocumentFilterPattern::Union2::as_string() { return detail::NodeAsPrimitive<Pattern>(n_); }
void TextDocumentFilterPattern::Union2::set_string(Pattern val) { detail::SetPrimitive(n_, val); }
bool TextDocumentFilterPattern::Union2::holds_RelativePattern() const { return n_.has_child("TODO"); }
RelativePattern TextDocumentFilterPattern::Union2::as_RelativePattern() { return {n_}; }
TextDocumentFilterPattern::Union2 TextDocumentFilterPattern::pattern() { return n_.find_child("pattern"); }
void TextDocumentFilterPattern::init() { StructWrapper::init(); TextDocumentFilterPattern::Union2(n_.append_child() << ryml::key("pattern")).init(); }
std::string_view NotebookDocumentFilterNotebookType::notebookType() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("notebookType")); }
void NotebookDocumentFilterNotebookType::set_notebookType(std::string_view val) { auto c = n_.find_child("notebookType"); if (c.invalid()) { c = n_.append_child() << ryml::key("notebookType"); } detail::SetPrimitive(c, val); }
std::string_view NotebookDocumentFilterNotebookType::scheme() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("scheme")); }
bool NotebookDocumentFilterNotebookType::has_scheme() const { return n_.has_child("scheme"); }
void NotebookDocumentFilterNotebookType::set_scheme(std::string_view val) { auto c = n_.find_child("scheme"); if (c.invalid()) { c = n_.append_child() << ryml::key("scheme"); } detail::SetPrimitive(c, val); }
bool NotebookDocumentFilterNotebookType::Union2::holds_string() const { return n_.has_child("TODO"); }
Pattern NotebookDocumentFilterNotebookType::Union2::as_string() { return detail::NodeAsPrimitive<Pattern>(n_); }
void NotebookDocumentFilterNotebookType::Union2::set_string(Pattern val) { detail::SetPrimitive(n_, val); }
bool NotebookDocumentFilterNotebookType::Union2::holds_RelativePattern() const { return n_.has_child("TODO"); }
RelativePattern NotebookDocumentFilterNotebookType::Union2::as_RelativePattern() { return {n_}; }
NotebookDocumentFilterNotebookType::Union2 NotebookDocumentFilterNotebookType::pattern() { return n_.find_child("pattern"); }
bool NotebookDocumentFilterNotebookType::has_pattern() const { return n_.has_child("pattern"); }
NotebookDocumentFilterNotebookType::Union2 NotebookDocumentFilterNotebookType::add_pattern() { auto w = NotebookDocumentFilterNotebookType::Union2(n_.append_child() << ryml::key("pattern")); w.init(); return w; }
void NotebookDocumentFilterNotebookType::init() { StructWrapper::init();  }
std::string_view NotebookDocumentFilterScheme::notebookType() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("notebookType")); }
bool NotebookDocumentFilterScheme::has_notebookType() const { return n_.has_child("notebookType"); }
void NotebookDocumentFilterScheme::set_notebookType(std::string_view val) { auto c = n_.find_child("notebookType"); if (c.invalid()) { c = n_.append_child() << ryml::key("notebookType"); } detail::SetPrimitive(c, val); }
std::string_view NotebookDocumentFilterScheme::scheme() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("scheme")); }
void NotebookDocumentFilterScheme::set_scheme(std::string_view val) { auto c = n_.find_child("scheme"); if (c.invalid()) { c = n_.append_child() << ryml::key("scheme"); } detail::SetPrimitive(c, val); }
bool NotebookDocumentFilterScheme::Union2::holds_string() const { return n_.has_child("TODO"); }
Pattern NotebookDocumentFilterScheme::Union2::as_string() { return detail::NodeAsPrimitive<Pattern>(n_); }
void NotebookDocumentFilterScheme::Union2::set_string(Pattern val) { detail::SetPrimitive(n_, val); }
bool NotebookDocumentFilterScheme::Union2::holds_RelativePattern() const { return n_.has_child("TODO"); }
RelativePattern NotebookDocumentFilterScheme::Union2::as_RelativePattern() { return {n_}; }
NotebookDocumentFilterScheme::Union2 NotebookDocumentFilterScheme::pattern() { return n_.find_child("pattern"); }
bool NotebookDocumentFilterScheme::has_pattern() const { return n_.has_child("pattern"); }
NotebookDocumentFilterScheme::Union2 NotebookDocumentFilterScheme::add_pattern() { auto w = NotebookDocumentFilterScheme::Union2(n_.append_child() << ryml::key("pattern")); w.init(); return w; }
void NotebookDocumentFilterScheme::init() { StructWrapper::init();  }
std::string_view NotebookDocumentFilterPattern::notebookType() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("notebookType")); }
bool NotebookDocumentFilterPattern::has_notebookType() const { return n_.has_child("notebookType"); }
void NotebookDocumentFilterPattern::set_notebookType(std::string_view val) { auto c = n_.find_child("notebookType"); if (c.invalid()) { c = n_.append_child() << ryml::key("notebookType"); } detail::SetPrimitive(c, val); }
std::string_view NotebookDocumentFilterPattern::scheme() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("scheme")); }
bool NotebookDocumentFilterPattern::has_scheme() const { return n_.has_child("scheme"); }
void NotebookDocumentFilterPattern::set_scheme(std::string_view val) { auto c = n_.find_child("scheme"); if (c.invalid()) { c = n_.append_child() << ryml::key("scheme"); } detail::SetPrimitive(c, val); }
bool NotebookDocumentFilterPattern::Union2::holds_string() const { return n_.has_child("TODO"); }
Pattern NotebookDocumentFilterPattern::Union2::as_string() { return detail::NodeAsPrimitive<Pattern>(n_); }
void NotebookDocumentFilterPattern::Union2::set_string(Pattern val) { detail::SetPrimitive(n_, val); }
bool NotebookDocumentFilterPattern::Union2::holds_RelativePattern() const { return n_.has_child("TODO"); }
RelativePattern NotebookDocumentFilterPattern::Union2::as_RelativePattern() { return {n_}; }
NotebookDocumentFilterPattern::Union2 NotebookDocumentFilterPattern::pattern() { return n_.find_child("pattern"); }
void NotebookDocumentFilterPattern::init() { StructWrapper::init(); NotebookDocumentFilterPattern::Union2(n_.append_child() << ryml::key("pattern")).init(); }
std::uint32_t NotebookCellArrayChange::start() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("start")); }
void NotebookCellArrayChange::set_start(std::uint32_t val) { auto c = n_.find_child("start"); if (c.invalid()) { c = n_.append_child() << ryml::key("start"); } detail::SetPrimitive(c, val); }
std::uint32_t NotebookCellArrayChange::deleteCount() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("deleteCount")); }
void NotebookCellArrayChange::set_deleteCount(std::uint32_t val) { auto c = n_.find_child("deleteCount"); if (c.invalid()) { c = n_.append_child() << ryml::key("deleteCount"); } detail::SetPrimitive(c, val); }
List<NotebookCell> NotebookCellArrayChange::cells() { return n_.find_child("cells"); }
bool NotebookCellArrayChange::has_cells() const { return n_.has_child("cells"); }
List<NotebookCell> NotebookCellArrayChange::add_cells() { auto w = List<NotebookCell>(n_.append_child() << ryml::key("cells")); w.init(); return w; }
void NotebookCellArrayChange::init() { StructWrapper::init();  }
bool WorkspaceEditClientCapabilities::documentChanges() { return detail::NodeAsPrimitive<bool>(n_.find_child("documentChanges")); }
bool WorkspaceEditClientCapabilities::has_documentChanges() const { return n_.has_child("documentChanges"); }
void WorkspaceEditClientCapabilities::set_documentChanges(bool val) { auto c = n_.find_child("documentChanges"); if (c.invalid()) { c = n_.append_child() << ryml::key("documentChanges"); } detail::SetPrimitive(c, val); }
List<ResourceOperationKind> WorkspaceEditClientCapabilities::resourceOperations() { return n_.find_child("resourceOperations"); }
bool WorkspaceEditClientCapabilities::has_resourceOperations() const { return n_.has_child("resourceOperations"); }
List<ResourceOperationKind> WorkspaceEditClientCapabilities::add_resourceOperations() { auto w = List<ResourceOperationKind>(n_.append_child() << ryml::key("resourceOperations")); w.init(); return w; }
FailureHandlingKind WorkspaceEditClientCapabilities::failureHandling() { return detail::DeserializeEnum<FailureHandlingKind>(n_.find_child("failureHandling")); }
bool WorkspaceEditClientCapabilities::has_failureHandling() const { return n_.has_child("failureHandling"); }
void WorkspaceEditClientCapabilities::set_failureHandling(FailureHandlingKind val) { auto c = n_.find_child("failureHandling"); if (c.invalid()) { c = n_.append_child() << ryml::key("failureHandling"); } detail::SerializeEnum(val, c); }
bool WorkspaceEditClientCapabilities::normalizesLineEndings() { return detail::NodeAsPrimitive<bool>(n_.find_child("normalizesLineEndings")); }
bool WorkspaceEditClientCapabilities::has_normalizesLineEndings() const { return n_.has_child("normalizesLineEndings"); }
void WorkspaceEditClientCapabilities::set_normalizesLineEndings(bool val) { auto c = n_.find_child("normalizesLineEndings"); if (c.invalid()) { c = n_.append_child() << ryml::key("normalizesLineEndings"); } detail::SetPrimitive(c, val); }
ChangeAnnotationsSupportOptions WorkspaceEditClientCapabilities::changeAnnotationSupport() { return n_.find_child("changeAnnotationSupport"); }
bool WorkspaceEditClientCapabilities::has_changeAnnotationSupport() const { return n_.has_child("changeAnnotationSupport"); }
ChangeAnnotationsSupportOptions WorkspaceEditClientCapabilities::add_changeAnnotationSupport() { auto w = ChangeAnnotationsSupportOptions(n_.append_child() << ryml::key("changeAnnotationSupport")); w.init(); return w; }
bool WorkspaceEditClientCapabilities::metadataSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("metadataSupport")); }
bool WorkspaceEditClientCapabilities::has_metadataSupport() const { return n_.has_child("metadataSupport"); }
void WorkspaceEditClientCapabilities::set_metadataSupport(bool val) { auto c = n_.find_child("metadataSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("metadataSupport"); } detail::SetPrimitive(c, val); }
bool WorkspaceEditClientCapabilities::snippetEditSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("snippetEditSupport")); }
bool WorkspaceEditClientCapabilities::has_snippetEditSupport() const { return n_.has_child("snippetEditSupport"); }
void WorkspaceEditClientCapabilities::set_snippetEditSupport(bool val) { auto c = n_.find_child("snippetEditSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("snippetEditSupport"); } detail::SetPrimitive(c, val); }
void WorkspaceEditClientCapabilities::init() { StructWrapper::init();  }
bool DidChangeConfigurationClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool DidChangeConfigurationClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void DidChangeConfigurationClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
void DidChangeConfigurationClientCapabilities::init() { StructWrapper::init();  }
bool DidChangeWatchedFilesClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool DidChangeWatchedFilesClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void DidChangeWatchedFilesClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
bool DidChangeWatchedFilesClientCapabilities::relativePatternSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("relativePatternSupport")); }
bool DidChangeWatchedFilesClientCapabilities::has_relativePatternSupport() const { return n_.has_child("relativePatternSupport"); }
void DidChangeWatchedFilesClientCapabilities::set_relativePatternSupport(bool val) { auto c = n_.find_child("relativePatternSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("relativePatternSupport"); } detail::SetPrimitive(c, val); }
void DidChangeWatchedFilesClientCapabilities::init() { StructWrapper::init();  }
bool WorkspaceSymbolClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool WorkspaceSymbolClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void WorkspaceSymbolClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
ClientSymbolKindOptions WorkspaceSymbolClientCapabilities::symbolKind() { return n_.find_child("symbolKind"); }
bool WorkspaceSymbolClientCapabilities::has_symbolKind() const { return n_.has_child("symbolKind"); }
ClientSymbolKindOptions WorkspaceSymbolClientCapabilities::add_symbolKind() { auto w = ClientSymbolKindOptions(n_.append_child() << ryml::key("symbolKind")); w.init(); return w; }
ClientSymbolTagOptions WorkspaceSymbolClientCapabilities::tagSupport() { return n_.find_child("tagSupport"); }
bool WorkspaceSymbolClientCapabilities::has_tagSupport() const { return n_.has_child("tagSupport"); }
ClientSymbolTagOptions WorkspaceSymbolClientCapabilities::add_tagSupport() { auto w = ClientSymbolTagOptions(n_.append_child() << ryml::key("tagSupport")); w.init(); return w; }
ClientSymbolResolveOptions WorkspaceSymbolClientCapabilities::resolveSupport() { return n_.find_child("resolveSupport"); }
bool WorkspaceSymbolClientCapabilities::has_resolveSupport() const { return n_.has_child("resolveSupport"); }
ClientSymbolResolveOptions WorkspaceSymbolClientCapabilities::add_resolveSupport() { auto w = ClientSymbolResolveOptions(n_.append_child() << ryml::key("resolveSupport")); w.init(); return w; }
void WorkspaceSymbolClientCapabilities::init() { StructWrapper::init();  }
bool ExecuteCommandClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool ExecuteCommandClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void ExecuteCommandClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
void ExecuteCommandClientCapabilities::init() { StructWrapper::init();  }
bool SemanticTokensWorkspaceClientCapabilities::refreshSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("refreshSupport")); }
bool SemanticTokensWorkspaceClientCapabilities::has_refreshSupport() const { return n_.has_child("refreshSupport"); }
void SemanticTokensWorkspaceClientCapabilities::set_refreshSupport(bool val) { auto c = n_.find_child("refreshSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("refreshSupport"); } detail::SetPrimitive(c, val); }
void SemanticTokensWorkspaceClientCapabilities::init() { StructWrapper::init();  }
bool CodeLensWorkspaceClientCapabilities::refreshSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("refreshSupport")); }
bool CodeLensWorkspaceClientCapabilities::has_refreshSupport() const { return n_.has_child("refreshSupport"); }
void CodeLensWorkspaceClientCapabilities::set_refreshSupport(bool val) { auto c = n_.find_child("refreshSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("refreshSupport"); } detail::SetPrimitive(c, val); }
void CodeLensWorkspaceClientCapabilities::init() { StructWrapper::init();  }
bool FileOperationClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool FileOperationClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void FileOperationClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
bool FileOperationClientCapabilities::didCreate() { return detail::NodeAsPrimitive<bool>(n_.find_child("didCreate")); }
bool FileOperationClientCapabilities::has_didCreate() const { return n_.has_child("didCreate"); }
void FileOperationClientCapabilities::set_didCreate(bool val) { auto c = n_.find_child("didCreate"); if (c.invalid()) { c = n_.append_child() << ryml::key("didCreate"); } detail::SetPrimitive(c, val); }
bool FileOperationClientCapabilities::willCreate() { return detail::NodeAsPrimitive<bool>(n_.find_child("willCreate")); }
bool FileOperationClientCapabilities::has_willCreate() const { return n_.has_child("willCreate"); }
void FileOperationClientCapabilities::set_willCreate(bool val) { auto c = n_.find_child("willCreate"); if (c.invalid()) { c = n_.append_child() << ryml::key("willCreate"); } detail::SetPrimitive(c, val); }
bool FileOperationClientCapabilities::didRename() { return detail::NodeAsPrimitive<bool>(n_.find_child("didRename")); }
bool FileOperationClientCapabilities::has_didRename() const { return n_.has_child("didRename"); }
void FileOperationClientCapabilities::set_didRename(bool val) { auto c = n_.find_child("didRename"); if (c.invalid()) { c = n_.append_child() << ryml::key("didRename"); } detail::SetPrimitive(c, val); }
bool FileOperationClientCapabilities::willRename() { return detail::NodeAsPrimitive<bool>(n_.find_child("willRename")); }
bool FileOperationClientCapabilities::has_willRename() const { return n_.has_child("willRename"); }
void FileOperationClientCapabilities::set_willRename(bool val) { auto c = n_.find_child("willRename"); if (c.invalid()) { c = n_.append_child() << ryml::key("willRename"); } detail::SetPrimitive(c, val); }
bool FileOperationClientCapabilities::didDelete() { return detail::NodeAsPrimitive<bool>(n_.find_child("didDelete")); }
bool FileOperationClientCapabilities::has_didDelete() const { return n_.has_child("didDelete"); }
void FileOperationClientCapabilities::set_didDelete(bool val) { auto c = n_.find_child("didDelete"); if (c.invalid()) { c = n_.append_child() << ryml::key("didDelete"); } detail::SetPrimitive(c, val); }
bool FileOperationClientCapabilities::willDelete() { return detail::NodeAsPrimitive<bool>(n_.find_child("willDelete")); }
bool FileOperationClientCapabilities::has_willDelete() const { return n_.has_child("willDelete"); }
void FileOperationClientCapabilities::set_willDelete(bool val) { auto c = n_.find_child("willDelete"); if (c.invalid()) { c = n_.append_child() << ryml::key("willDelete"); } detail::SetPrimitive(c, val); }
void FileOperationClientCapabilities::init() { StructWrapper::init();  }
bool InlineValueWorkspaceClientCapabilities::refreshSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("refreshSupport")); }
bool InlineValueWorkspaceClientCapabilities::has_refreshSupport() const { return n_.has_child("refreshSupport"); }
void InlineValueWorkspaceClientCapabilities::set_refreshSupport(bool val) { auto c = n_.find_child("refreshSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("refreshSupport"); } detail::SetPrimitive(c, val); }
void InlineValueWorkspaceClientCapabilities::init() { StructWrapper::init();  }
bool InlayHintWorkspaceClientCapabilities::refreshSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("refreshSupport")); }
bool InlayHintWorkspaceClientCapabilities::has_refreshSupport() const { return n_.has_child("refreshSupport"); }
void InlayHintWorkspaceClientCapabilities::set_refreshSupport(bool val) { auto c = n_.find_child("refreshSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("refreshSupport"); } detail::SetPrimitive(c, val); }
void InlayHintWorkspaceClientCapabilities::init() { StructWrapper::init();  }
bool DiagnosticWorkspaceClientCapabilities::refreshSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("refreshSupport")); }
bool DiagnosticWorkspaceClientCapabilities::has_refreshSupport() const { return n_.has_child("refreshSupport"); }
void DiagnosticWorkspaceClientCapabilities::set_refreshSupport(bool val) { auto c = n_.find_child("refreshSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("refreshSupport"); } detail::SetPrimitive(c, val); }
void DiagnosticWorkspaceClientCapabilities::init() { StructWrapper::init();  }
bool FoldingRangeWorkspaceClientCapabilities::refreshSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("refreshSupport")); }
bool FoldingRangeWorkspaceClientCapabilities::has_refreshSupport() const { return n_.has_child("refreshSupport"); }
void FoldingRangeWorkspaceClientCapabilities::set_refreshSupport(bool val) { auto c = n_.find_child("refreshSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("refreshSupport"); } detail::SetPrimitive(c, val); }
void FoldingRangeWorkspaceClientCapabilities::init() { StructWrapper::init();  }
bool TextDocumentContentClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool TextDocumentContentClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void TextDocumentContentClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
void TextDocumentContentClientCapabilities::init() { StructWrapper::init();  }
bool TextDocumentSyncClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool TextDocumentSyncClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void TextDocumentSyncClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
bool TextDocumentSyncClientCapabilities::willSave() { return detail::NodeAsPrimitive<bool>(n_.find_child("willSave")); }
bool TextDocumentSyncClientCapabilities::has_willSave() const { return n_.has_child("willSave"); }
void TextDocumentSyncClientCapabilities::set_willSave(bool val) { auto c = n_.find_child("willSave"); if (c.invalid()) { c = n_.append_child() << ryml::key("willSave"); } detail::SetPrimitive(c, val); }
bool TextDocumentSyncClientCapabilities::willSaveWaitUntil() { return detail::NodeAsPrimitive<bool>(n_.find_child("willSaveWaitUntil")); }
bool TextDocumentSyncClientCapabilities::has_willSaveWaitUntil() const { return n_.has_child("willSaveWaitUntil"); }
void TextDocumentSyncClientCapabilities::set_willSaveWaitUntil(bool val) { auto c = n_.find_child("willSaveWaitUntil"); if (c.invalid()) { c = n_.append_child() << ryml::key("willSaveWaitUntil"); } detail::SetPrimitive(c, val); }
bool TextDocumentSyncClientCapabilities::didSave() { return detail::NodeAsPrimitive<bool>(n_.find_child("didSave")); }
bool TextDocumentSyncClientCapabilities::has_didSave() const { return n_.has_child("didSave"); }
void TextDocumentSyncClientCapabilities::set_didSave(bool val) { auto c = n_.find_child("didSave"); if (c.invalid()) { c = n_.append_child() << ryml::key("didSave"); } detail::SetPrimitive(c, val); }
void TextDocumentSyncClientCapabilities::init() { StructWrapper::init();  }
bool TextDocumentFilterClientCapabilities::relativePatternSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("relativePatternSupport")); }
bool TextDocumentFilterClientCapabilities::has_relativePatternSupport() const { return n_.has_child("relativePatternSupport"); }
void TextDocumentFilterClientCapabilities::set_relativePatternSupport(bool val) { auto c = n_.find_child("relativePatternSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("relativePatternSupport"); } detail::SetPrimitive(c, val); }
void TextDocumentFilterClientCapabilities::init() { StructWrapper::init();  }
bool CompletionClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool CompletionClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void CompletionClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
ClientCompletionItemOptions CompletionClientCapabilities::completionItem() { return n_.find_child("completionItem"); }
bool CompletionClientCapabilities::has_completionItem() const { return n_.has_child("completionItem"); }
ClientCompletionItemOptions CompletionClientCapabilities::add_completionItem() { auto w = ClientCompletionItemOptions(n_.append_child() << ryml::key("completionItem")); w.init(); return w; }
ClientCompletionItemOptionsKind CompletionClientCapabilities::completionItemKind() { return n_.find_child("completionItemKind"); }
bool CompletionClientCapabilities::has_completionItemKind() const { return n_.has_child("completionItemKind"); }
ClientCompletionItemOptionsKind CompletionClientCapabilities::add_completionItemKind() { auto w = ClientCompletionItemOptionsKind(n_.append_child() << ryml::key("completionItemKind")); w.init(); return w; }
InsertTextMode CompletionClientCapabilities::insertTextMode() { return detail::DeserializeEnum<InsertTextMode>(n_.find_child("insertTextMode")); }
bool CompletionClientCapabilities::has_insertTextMode() const { return n_.has_child("insertTextMode"); }
void CompletionClientCapabilities::set_insertTextMode(InsertTextMode val) { auto c = n_.find_child("insertTextMode"); if (c.invalid()) { c = n_.append_child() << ryml::key("insertTextMode"); } detail::SerializeEnum(val, c); }
bool CompletionClientCapabilities::contextSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("contextSupport")); }
bool CompletionClientCapabilities::has_contextSupport() const { return n_.has_child("contextSupport"); }
void CompletionClientCapabilities::set_contextSupport(bool val) { auto c = n_.find_child("contextSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("contextSupport"); } detail::SetPrimitive(c, val); }
CompletionListCapabilities CompletionClientCapabilities::completionList() { return n_.find_child("completionList"); }
bool CompletionClientCapabilities::has_completionList() const { return n_.has_child("completionList"); }
CompletionListCapabilities CompletionClientCapabilities::add_completionList() { auto w = CompletionListCapabilities(n_.append_child() << ryml::key("completionList")); w.init(); return w; }
void CompletionClientCapabilities::init() { StructWrapper::init();  }
bool HoverClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool HoverClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void HoverClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
List<MarkupKind> HoverClientCapabilities::contentFormat() { return n_.find_child("contentFormat"); }
bool HoverClientCapabilities::has_contentFormat() const { return n_.has_child("contentFormat"); }
List<MarkupKind> HoverClientCapabilities::add_contentFormat() { auto w = List<MarkupKind>(n_.append_child() << ryml::key("contentFormat")); w.init(); return w; }
void HoverClientCapabilities::init() { StructWrapper::init();  }
bool SignatureHelpClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool SignatureHelpClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void SignatureHelpClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
ClientSignatureInformationOptions SignatureHelpClientCapabilities::signatureInformation() { return n_.find_child("signatureInformation"); }
bool SignatureHelpClientCapabilities::has_signatureInformation() const { return n_.has_child("signatureInformation"); }
ClientSignatureInformationOptions SignatureHelpClientCapabilities::add_signatureInformation() { auto w = ClientSignatureInformationOptions(n_.append_child() << ryml::key("signatureInformation")); w.init(); return w; }
bool SignatureHelpClientCapabilities::contextSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("contextSupport")); }
bool SignatureHelpClientCapabilities::has_contextSupport() const { return n_.has_child("contextSupport"); }
void SignatureHelpClientCapabilities::set_contextSupport(bool val) { auto c = n_.find_child("contextSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("contextSupport"); } detail::SetPrimitive(c, val); }
void SignatureHelpClientCapabilities::init() { StructWrapper::init();  }
bool DeclarationClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool DeclarationClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void DeclarationClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
bool DeclarationClientCapabilities::linkSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("linkSupport")); }
bool DeclarationClientCapabilities::has_linkSupport() const { return n_.has_child("linkSupport"); }
void DeclarationClientCapabilities::set_linkSupport(bool val) { auto c = n_.find_child("linkSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("linkSupport"); } detail::SetPrimitive(c, val); }
void DeclarationClientCapabilities::init() { StructWrapper::init();  }
bool DefinitionClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool DefinitionClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void DefinitionClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
bool DefinitionClientCapabilities::linkSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("linkSupport")); }
bool DefinitionClientCapabilities::has_linkSupport() const { return n_.has_child("linkSupport"); }
void DefinitionClientCapabilities::set_linkSupport(bool val) { auto c = n_.find_child("linkSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("linkSupport"); } detail::SetPrimitive(c, val); }
void DefinitionClientCapabilities::init() { StructWrapper::init();  }
bool TypeDefinitionClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool TypeDefinitionClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void TypeDefinitionClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
bool TypeDefinitionClientCapabilities::linkSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("linkSupport")); }
bool TypeDefinitionClientCapabilities::has_linkSupport() const { return n_.has_child("linkSupport"); }
void TypeDefinitionClientCapabilities::set_linkSupport(bool val) { auto c = n_.find_child("linkSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("linkSupport"); } detail::SetPrimitive(c, val); }
void TypeDefinitionClientCapabilities::init() { StructWrapper::init();  }
bool ImplementationClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool ImplementationClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void ImplementationClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
bool ImplementationClientCapabilities::linkSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("linkSupport")); }
bool ImplementationClientCapabilities::has_linkSupport() const { return n_.has_child("linkSupport"); }
void ImplementationClientCapabilities::set_linkSupport(bool val) { auto c = n_.find_child("linkSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("linkSupport"); } detail::SetPrimitive(c, val); }
void ImplementationClientCapabilities::init() { StructWrapper::init();  }
bool ReferenceClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool ReferenceClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void ReferenceClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
void ReferenceClientCapabilities::init() { StructWrapper::init();  }
bool DocumentHighlightClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool DocumentHighlightClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void DocumentHighlightClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
void DocumentHighlightClientCapabilities::init() { StructWrapper::init();  }
bool DocumentSymbolClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool DocumentSymbolClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void DocumentSymbolClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
ClientSymbolKindOptions DocumentSymbolClientCapabilities::symbolKind() { return n_.find_child("symbolKind"); }
bool DocumentSymbolClientCapabilities::has_symbolKind() const { return n_.has_child("symbolKind"); }
ClientSymbolKindOptions DocumentSymbolClientCapabilities::add_symbolKind() { auto w = ClientSymbolKindOptions(n_.append_child() << ryml::key("symbolKind")); w.init(); return w; }
bool DocumentSymbolClientCapabilities::hierarchicalDocumentSymbolSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("hierarchicalDocumentSymbolSupport")); }
bool DocumentSymbolClientCapabilities::has_hierarchicalDocumentSymbolSupport() const { return n_.has_child("hierarchicalDocumentSymbolSupport"); }
void DocumentSymbolClientCapabilities::set_hierarchicalDocumentSymbolSupport(bool val) { auto c = n_.find_child("hierarchicalDocumentSymbolSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("hierarchicalDocumentSymbolSupport"); } detail::SetPrimitive(c, val); }
ClientSymbolTagOptions DocumentSymbolClientCapabilities::tagSupport() { return n_.find_child("tagSupport"); }
bool DocumentSymbolClientCapabilities::has_tagSupport() const { return n_.has_child("tagSupport"); }
ClientSymbolTagOptions DocumentSymbolClientCapabilities::add_tagSupport() { auto w = ClientSymbolTagOptions(n_.append_child() << ryml::key("tagSupport")); w.init(); return w; }
bool DocumentSymbolClientCapabilities::labelSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("labelSupport")); }
bool DocumentSymbolClientCapabilities::has_labelSupport() const { return n_.has_child("labelSupport"); }
void DocumentSymbolClientCapabilities::set_labelSupport(bool val) { auto c = n_.find_child("labelSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("labelSupport"); } detail::SetPrimitive(c, val); }
void DocumentSymbolClientCapabilities::init() { StructWrapper::init();  }
bool CodeActionClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool CodeActionClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void CodeActionClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
ClientCodeActionLiteralOptions CodeActionClientCapabilities::codeActionLiteralSupport() { return n_.find_child("codeActionLiteralSupport"); }
bool CodeActionClientCapabilities::has_codeActionLiteralSupport() const { return n_.has_child("codeActionLiteralSupport"); }
ClientCodeActionLiteralOptions CodeActionClientCapabilities::add_codeActionLiteralSupport() { auto w = ClientCodeActionLiteralOptions(n_.append_child() << ryml::key("codeActionLiteralSupport")); w.init(); return w; }
bool CodeActionClientCapabilities::isPreferredSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("isPreferredSupport")); }
bool CodeActionClientCapabilities::has_isPreferredSupport() const { return n_.has_child("isPreferredSupport"); }
void CodeActionClientCapabilities::set_isPreferredSupport(bool val) { auto c = n_.find_child("isPreferredSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("isPreferredSupport"); } detail::SetPrimitive(c, val); }
bool CodeActionClientCapabilities::disabledSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("disabledSupport")); }
bool CodeActionClientCapabilities::has_disabledSupport() const { return n_.has_child("disabledSupport"); }
void CodeActionClientCapabilities::set_disabledSupport(bool val) { auto c = n_.find_child("disabledSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("disabledSupport"); } detail::SetPrimitive(c, val); }
bool CodeActionClientCapabilities::dataSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("dataSupport")); }
bool CodeActionClientCapabilities::has_dataSupport() const { return n_.has_child("dataSupport"); }
void CodeActionClientCapabilities::set_dataSupport(bool val) { auto c = n_.find_child("dataSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("dataSupport"); } detail::SetPrimitive(c, val); }
ClientCodeActionResolveOptions CodeActionClientCapabilities::resolveSupport() { return n_.find_child("resolveSupport"); }
bool CodeActionClientCapabilities::has_resolveSupport() const { return n_.has_child("resolveSupport"); }
ClientCodeActionResolveOptions CodeActionClientCapabilities::add_resolveSupport() { auto w = ClientCodeActionResolveOptions(n_.append_child() << ryml::key("resolveSupport")); w.init(); return w; }
bool CodeActionClientCapabilities::honorsChangeAnnotations() { return detail::NodeAsPrimitive<bool>(n_.find_child("honorsChangeAnnotations")); }
bool CodeActionClientCapabilities::has_honorsChangeAnnotations() const { return n_.has_child("honorsChangeAnnotations"); }
void CodeActionClientCapabilities::set_honorsChangeAnnotations(bool val) { auto c = n_.find_child("honorsChangeAnnotations"); if (c.invalid()) { c = n_.append_child() << ryml::key("honorsChangeAnnotations"); } detail::SetPrimitive(c, val); }
bool CodeActionClientCapabilities::documentationSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("documentationSupport")); }
bool CodeActionClientCapabilities::has_documentationSupport() const { return n_.has_child("documentationSupport"); }
void CodeActionClientCapabilities::set_documentationSupport(bool val) { auto c = n_.find_child("documentationSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("documentationSupport"); } detail::SetPrimitive(c, val); }
CodeActionTagOptions CodeActionClientCapabilities::tagSupport() { return n_.find_child("tagSupport"); }
bool CodeActionClientCapabilities::has_tagSupport() const { return n_.has_child("tagSupport"); }
CodeActionTagOptions CodeActionClientCapabilities::add_tagSupport() { auto w = CodeActionTagOptions(n_.append_child() << ryml::key("tagSupport")); w.init(); return w; }
void CodeActionClientCapabilities::init() { StructWrapper::init();  }
bool CodeLensClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool CodeLensClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void CodeLensClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
ClientCodeLensResolveOptions CodeLensClientCapabilities::resolveSupport() { return n_.find_child("resolveSupport"); }
bool CodeLensClientCapabilities::has_resolveSupport() const { return n_.has_child("resolveSupport"); }
ClientCodeLensResolveOptions CodeLensClientCapabilities::add_resolveSupport() { auto w = ClientCodeLensResolveOptions(n_.append_child() << ryml::key("resolveSupport")); w.init(); return w; }
void CodeLensClientCapabilities::init() { StructWrapper::init();  }
bool DocumentLinkClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool DocumentLinkClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void DocumentLinkClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
bool DocumentLinkClientCapabilities::tooltipSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("tooltipSupport")); }
bool DocumentLinkClientCapabilities::has_tooltipSupport() const { return n_.has_child("tooltipSupport"); }
void DocumentLinkClientCapabilities::set_tooltipSupport(bool val) { auto c = n_.find_child("tooltipSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("tooltipSupport"); } detail::SetPrimitive(c, val); }
void DocumentLinkClientCapabilities::init() { StructWrapper::init();  }
bool DocumentColorClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool DocumentColorClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void DocumentColorClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
void DocumentColorClientCapabilities::init() { StructWrapper::init();  }
bool DocumentFormattingClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool DocumentFormattingClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void DocumentFormattingClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
void DocumentFormattingClientCapabilities::init() { StructWrapper::init();  }
bool DocumentRangeFormattingClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool DocumentRangeFormattingClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void DocumentRangeFormattingClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
bool DocumentRangeFormattingClientCapabilities::rangesSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("rangesSupport")); }
bool DocumentRangeFormattingClientCapabilities::has_rangesSupport() const { return n_.has_child("rangesSupport"); }
void DocumentRangeFormattingClientCapabilities::set_rangesSupport(bool val) { auto c = n_.find_child("rangesSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("rangesSupport"); } detail::SetPrimitive(c, val); }
void DocumentRangeFormattingClientCapabilities::init() { StructWrapper::init();  }
bool DocumentOnTypeFormattingClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool DocumentOnTypeFormattingClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void DocumentOnTypeFormattingClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
void DocumentOnTypeFormattingClientCapabilities::init() { StructWrapper::init();  }
bool RenameClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool RenameClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void RenameClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
bool RenameClientCapabilities::prepareSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("prepareSupport")); }
bool RenameClientCapabilities::has_prepareSupport() const { return n_.has_child("prepareSupport"); }
void RenameClientCapabilities::set_prepareSupport(bool val) { auto c = n_.find_child("prepareSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("prepareSupport"); } detail::SetPrimitive(c, val); }
PrepareSupportDefaultBehavior RenameClientCapabilities::prepareSupportDefaultBehavior() { return detail::DeserializeEnum<PrepareSupportDefaultBehavior>(n_.find_child("prepareSupportDefaultBehavior")); }
bool RenameClientCapabilities::has_prepareSupportDefaultBehavior() const { return n_.has_child("prepareSupportDefaultBehavior"); }
void RenameClientCapabilities::set_prepareSupportDefaultBehavior(PrepareSupportDefaultBehavior val) { auto c = n_.find_child("prepareSupportDefaultBehavior"); if (c.invalid()) { c = n_.append_child() << ryml::key("prepareSupportDefaultBehavior"); } detail::SerializeEnum(val, c); }
bool RenameClientCapabilities::honorsChangeAnnotations() { return detail::NodeAsPrimitive<bool>(n_.find_child("honorsChangeAnnotations")); }
bool RenameClientCapabilities::has_honorsChangeAnnotations() const { return n_.has_child("honorsChangeAnnotations"); }
void RenameClientCapabilities::set_honorsChangeAnnotations(bool val) { auto c = n_.find_child("honorsChangeAnnotations"); if (c.invalid()) { c = n_.append_child() << ryml::key("honorsChangeAnnotations"); } detail::SetPrimitive(c, val); }
void RenameClientCapabilities::init() { StructWrapper::init();  }
bool FoldingRangeClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool FoldingRangeClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void FoldingRangeClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
std::uint32_t FoldingRangeClientCapabilities::rangeLimit() { return detail::NodeAsPrimitive<std::uint32_t>(n_.find_child("rangeLimit")); }
bool FoldingRangeClientCapabilities::has_rangeLimit() const { return n_.has_child("rangeLimit"); }
void FoldingRangeClientCapabilities::set_rangeLimit(std::uint32_t val) { auto c = n_.find_child("rangeLimit"); if (c.invalid()) { c = n_.append_child() << ryml::key("rangeLimit"); } detail::SetPrimitive(c, val); }
bool FoldingRangeClientCapabilities::lineFoldingOnly() { return detail::NodeAsPrimitive<bool>(n_.find_child("lineFoldingOnly")); }
bool FoldingRangeClientCapabilities::has_lineFoldingOnly() const { return n_.has_child("lineFoldingOnly"); }
void FoldingRangeClientCapabilities::set_lineFoldingOnly(bool val) { auto c = n_.find_child("lineFoldingOnly"); if (c.invalid()) { c = n_.append_child() << ryml::key("lineFoldingOnly"); } detail::SetPrimitive(c, val); }
ClientFoldingRangeKindOptions FoldingRangeClientCapabilities::foldingRangeKind() { return n_.find_child("foldingRangeKind"); }
bool FoldingRangeClientCapabilities::has_foldingRangeKind() const { return n_.has_child("foldingRangeKind"); }
ClientFoldingRangeKindOptions FoldingRangeClientCapabilities::add_foldingRangeKind() { auto w = ClientFoldingRangeKindOptions(n_.append_child() << ryml::key("foldingRangeKind")); w.init(); return w; }
ClientFoldingRangeOptions FoldingRangeClientCapabilities::foldingRange() { return n_.find_child("foldingRange"); }
bool FoldingRangeClientCapabilities::has_foldingRange() const { return n_.has_child("foldingRange"); }
ClientFoldingRangeOptions FoldingRangeClientCapabilities::add_foldingRange() { auto w = ClientFoldingRangeOptions(n_.append_child() << ryml::key("foldingRange")); w.init(); return w; }
void FoldingRangeClientCapabilities::init() { StructWrapper::init();  }
bool SelectionRangeClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool SelectionRangeClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void SelectionRangeClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
void SelectionRangeClientCapabilities::init() { StructWrapper::init();  }
bool PublishDiagnosticsClientCapabilities::versionSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("versionSupport")); }
bool PublishDiagnosticsClientCapabilities::has_versionSupport() const { return n_.has_child("versionSupport"); }
void PublishDiagnosticsClientCapabilities::set_versionSupport(bool val) { auto c = n_.find_child("versionSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("versionSupport"); } detail::SetPrimitive(c, val); }
bool PublishDiagnosticsClientCapabilities::relatedInformation() { return detail::NodeAsPrimitive<bool>(n_.find_child("relatedInformation")); }
bool PublishDiagnosticsClientCapabilities::has_relatedInformation() const { return n_.has_child("relatedInformation"); }
void PublishDiagnosticsClientCapabilities::set_relatedInformation(bool val) { auto c = n_.find_child("relatedInformation"); if (c.invalid()) { c = n_.append_child() << ryml::key("relatedInformation"); } detail::SetPrimitive(c, val); }
ClientDiagnosticsTagOptions PublishDiagnosticsClientCapabilities::tagSupport() { return n_.find_child("tagSupport"); }
bool PublishDiagnosticsClientCapabilities::has_tagSupport() const { return n_.has_child("tagSupport"); }
ClientDiagnosticsTagOptions PublishDiagnosticsClientCapabilities::add_tagSupport() { auto w = ClientDiagnosticsTagOptions(n_.append_child() << ryml::key("tagSupport")); w.init(); return w; }
bool PublishDiagnosticsClientCapabilities::codeDescriptionSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("codeDescriptionSupport")); }
bool PublishDiagnosticsClientCapabilities::has_codeDescriptionSupport() const { return n_.has_child("codeDescriptionSupport"); }
void PublishDiagnosticsClientCapabilities::set_codeDescriptionSupport(bool val) { auto c = n_.find_child("codeDescriptionSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("codeDescriptionSupport"); } detail::SetPrimitive(c, val); }
bool PublishDiagnosticsClientCapabilities::dataSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("dataSupport")); }
bool PublishDiagnosticsClientCapabilities::has_dataSupport() const { return n_.has_child("dataSupport"); }
void PublishDiagnosticsClientCapabilities::set_dataSupport(bool val) { auto c = n_.find_child("dataSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("dataSupport"); } detail::SetPrimitive(c, val); }
void PublishDiagnosticsClientCapabilities::init() { StructWrapper::init();  }
bool CallHierarchyClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool CallHierarchyClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void CallHierarchyClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
void CallHierarchyClientCapabilities::init() { StructWrapper::init();  }
bool SemanticTokensClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool SemanticTokensClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void SemanticTokensClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
ClientSemanticTokensRequestOptions SemanticTokensClientCapabilities::requests() { return n_.find_child("requests"); }
List<std::string_view> SemanticTokensClientCapabilities::tokenTypes() { return n_.find_child("tokenTypes"); }
List<std::string_view> SemanticTokensClientCapabilities::tokenModifiers() { return n_.find_child("tokenModifiers"); }
List<TokenFormat> SemanticTokensClientCapabilities::formats() { return n_.find_child("formats"); }
bool SemanticTokensClientCapabilities::overlappingTokenSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("overlappingTokenSupport")); }
bool SemanticTokensClientCapabilities::has_overlappingTokenSupport() const { return n_.has_child("overlappingTokenSupport"); }
void SemanticTokensClientCapabilities::set_overlappingTokenSupport(bool val) { auto c = n_.find_child("overlappingTokenSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("overlappingTokenSupport"); } detail::SetPrimitive(c, val); }
bool SemanticTokensClientCapabilities::multilineTokenSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("multilineTokenSupport")); }
bool SemanticTokensClientCapabilities::has_multilineTokenSupport() const { return n_.has_child("multilineTokenSupport"); }
void SemanticTokensClientCapabilities::set_multilineTokenSupport(bool val) { auto c = n_.find_child("multilineTokenSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("multilineTokenSupport"); } detail::SetPrimitive(c, val); }
bool SemanticTokensClientCapabilities::serverCancelSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("serverCancelSupport")); }
bool SemanticTokensClientCapabilities::has_serverCancelSupport() const { return n_.has_child("serverCancelSupport"); }
void SemanticTokensClientCapabilities::set_serverCancelSupport(bool val) { auto c = n_.find_child("serverCancelSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("serverCancelSupport"); } detail::SetPrimitive(c, val); }
bool SemanticTokensClientCapabilities::augmentsSyntaxTokens() { return detail::NodeAsPrimitive<bool>(n_.find_child("augmentsSyntaxTokens")); }
bool SemanticTokensClientCapabilities::has_augmentsSyntaxTokens() const { return n_.has_child("augmentsSyntaxTokens"); }
void SemanticTokensClientCapabilities::set_augmentsSyntaxTokens(bool val) { auto c = n_.find_child("augmentsSyntaxTokens"); if (c.invalid()) { c = n_.append_child() << ryml::key("augmentsSyntaxTokens"); } detail::SetPrimitive(c, val); }
void SemanticTokensClientCapabilities::init() { StructWrapper::init(); ClientSemanticTokensRequestOptions(n_.append_child() << ryml::key("requests")).init(); List<std::string_view>(n_.append_child() << ryml::key("tokenTypes")).init(); List<std::string_view>(n_.append_child() << ryml::key("tokenModifiers")).init(); List<TokenFormat>(n_.append_child() << ryml::key("formats")).init(); }
bool LinkedEditingRangeClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool LinkedEditingRangeClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void LinkedEditingRangeClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
void LinkedEditingRangeClientCapabilities::init() { StructWrapper::init();  }
bool MonikerClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool MonikerClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void MonikerClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
void MonikerClientCapabilities::init() { StructWrapper::init();  }
bool TypeHierarchyClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool TypeHierarchyClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void TypeHierarchyClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
void TypeHierarchyClientCapabilities::init() { StructWrapper::init();  }
bool InlineValueClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool InlineValueClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void InlineValueClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
void InlineValueClientCapabilities::init() { StructWrapper::init();  }
bool InlayHintClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool InlayHintClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void InlayHintClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
ClientInlayHintResolveOptions InlayHintClientCapabilities::resolveSupport() { return n_.find_child("resolveSupport"); }
bool InlayHintClientCapabilities::has_resolveSupport() const { return n_.has_child("resolveSupport"); }
ClientInlayHintResolveOptions InlayHintClientCapabilities::add_resolveSupport() { auto w = ClientInlayHintResolveOptions(n_.append_child() << ryml::key("resolveSupport")); w.init(); return w; }
void InlayHintClientCapabilities::init() { StructWrapper::init();  }
bool DiagnosticClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool DiagnosticClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void DiagnosticClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
bool DiagnosticClientCapabilities::relatedDocumentSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("relatedDocumentSupport")); }
bool DiagnosticClientCapabilities::has_relatedDocumentSupport() const { return n_.has_child("relatedDocumentSupport"); }
void DiagnosticClientCapabilities::set_relatedDocumentSupport(bool val) { auto c = n_.find_child("relatedDocumentSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("relatedDocumentSupport"); } detail::SetPrimitive(c, val); }
bool DiagnosticClientCapabilities::relatedInformation() { return detail::NodeAsPrimitive<bool>(n_.find_child("relatedInformation")); }
bool DiagnosticClientCapabilities::has_relatedInformation() const { return n_.has_child("relatedInformation"); }
void DiagnosticClientCapabilities::set_relatedInformation(bool val) { auto c = n_.find_child("relatedInformation"); if (c.invalid()) { c = n_.append_child() << ryml::key("relatedInformation"); } detail::SetPrimitive(c, val); }
ClientDiagnosticsTagOptions DiagnosticClientCapabilities::tagSupport() { return n_.find_child("tagSupport"); }
bool DiagnosticClientCapabilities::has_tagSupport() const { return n_.has_child("tagSupport"); }
ClientDiagnosticsTagOptions DiagnosticClientCapabilities::add_tagSupport() { auto w = ClientDiagnosticsTagOptions(n_.append_child() << ryml::key("tagSupport")); w.init(); return w; }
bool DiagnosticClientCapabilities::codeDescriptionSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("codeDescriptionSupport")); }
bool DiagnosticClientCapabilities::has_codeDescriptionSupport() const { return n_.has_child("codeDescriptionSupport"); }
void DiagnosticClientCapabilities::set_codeDescriptionSupport(bool val) { auto c = n_.find_child("codeDescriptionSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("codeDescriptionSupport"); } detail::SetPrimitive(c, val); }
bool DiagnosticClientCapabilities::dataSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("dataSupport")); }
bool DiagnosticClientCapabilities::has_dataSupport() const { return n_.has_child("dataSupport"); }
void DiagnosticClientCapabilities::set_dataSupport(bool val) { auto c = n_.find_child("dataSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("dataSupport"); } detail::SetPrimitive(c, val); }
void DiagnosticClientCapabilities::init() { StructWrapper::init();  }
bool InlineCompletionClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool InlineCompletionClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void InlineCompletionClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
void InlineCompletionClientCapabilities::init() { StructWrapper::init();  }
bool NotebookDocumentSyncClientCapabilities::dynamicRegistration() { return detail::NodeAsPrimitive<bool>(n_.find_child("dynamicRegistration")); }
bool NotebookDocumentSyncClientCapabilities::has_dynamicRegistration() const { return n_.has_child("dynamicRegistration"); }
void NotebookDocumentSyncClientCapabilities::set_dynamicRegistration(bool val) { auto c = n_.find_child("dynamicRegistration"); if (c.invalid()) { c = n_.append_child() << ryml::key("dynamicRegistration"); } detail::SetPrimitive(c, val); }
bool NotebookDocumentSyncClientCapabilities::executionSummarySupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("executionSummarySupport")); }
bool NotebookDocumentSyncClientCapabilities::has_executionSummarySupport() const { return n_.has_child("executionSummarySupport"); }
void NotebookDocumentSyncClientCapabilities::set_executionSummarySupport(bool val) { auto c = n_.find_child("executionSummarySupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("executionSummarySupport"); } detail::SetPrimitive(c, val); }
void NotebookDocumentSyncClientCapabilities::init() { StructWrapper::init();  }
ClientShowMessageActionItemOptions ShowMessageRequestClientCapabilities::messageActionItem() { return n_.find_child("messageActionItem"); }
bool ShowMessageRequestClientCapabilities::has_messageActionItem() const { return n_.has_child("messageActionItem"); }
ClientShowMessageActionItemOptions ShowMessageRequestClientCapabilities::add_messageActionItem() { auto w = ClientShowMessageActionItemOptions(n_.append_child() << ryml::key("messageActionItem")); w.init(); return w; }
void ShowMessageRequestClientCapabilities::init() { StructWrapper::init();  }
bool ShowDocumentClientCapabilities::support() { return detail::NodeAsPrimitive<bool>(n_.find_child("support")); }
void ShowDocumentClientCapabilities::set_support(bool val) { auto c = n_.find_child("support"); if (c.invalid()) { c = n_.append_child() << ryml::key("support"); } detail::SetPrimitive(c, val); }
void ShowDocumentClientCapabilities::init() { StructWrapper::init();  }
bool StaleRequestSupportOptions::cancel() { return detail::NodeAsPrimitive<bool>(n_.find_child("cancel")); }
void StaleRequestSupportOptions::set_cancel(bool val) { auto c = n_.find_child("cancel"); if (c.invalid()) { c = n_.append_child() << ryml::key("cancel"); } detail::SetPrimitive(c, val); }
List<std::string_view> StaleRequestSupportOptions::retryOnContentModified() { return n_.find_child("retryOnContentModified"); }
void StaleRequestSupportOptions::init() { StructWrapper::init(); List<std::string_view>(n_.append_child() << ryml::key("retryOnContentModified")).init(); }
RegularExpressionEngineKind RegularExpressionsClientCapabilities::engine() { return detail::NodeAsPrimitive<RegularExpressionEngineKind>(n_.find_child("engine")); }
void RegularExpressionsClientCapabilities::set_engine(RegularExpressionEngineKind val) { auto c = n_.find_child("engine"); if (c.invalid()) { c = n_.append_child() << ryml::key("engine"); } detail::SetPrimitive(c, val); }
std::string_view RegularExpressionsClientCapabilities::version() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("version")); }
bool RegularExpressionsClientCapabilities::has_version() const { return n_.has_child("version"); }
void RegularExpressionsClientCapabilities::set_version(std::string_view val) { auto c = n_.find_child("version"); if (c.invalid()) { c = n_.append_child() << ryml::key("version"); } detail::SetPrimitive(c, val); }
void RegularExpressionsClientCapabilities::init() { StructWrapper::init();  }
std::string_view MarkdownClientCapabilities::parser() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("parser")); }
void MarkdownClientCapabilities::set_parser(std::string_view val) { auto c = n_.find_child("parser"); if (c.invalid()) { c = n_.append_child() << ryml::key("parser"); } detail::SetPrimitive(c, val); }
std::string_view MarkdownClientCapabilities::version() { return detail::NodeAsPrimitive<std::string_view>(n_.find_child("version")); }
bool MarkdownClientCapabilities::has_version() const { return n_.has_child("version"); }
void MarkdownClientCapabilities::set_version(std::string_view val) { auto c = n_.find_child("version"); if (c.invalid()) { c = n_.append_child() << ryml::key("version"); } detail::SetPrimitive(c, val); }
List<std::string_view> MarkdownClientCapabilities::allowedTags() { return n_.find_child("allowedTags"); }
bool MarkdownClientCapabilities::has_allowedTags() const { return n_.has_child("allowedTags"); }
List<std::string_view> MarkdownClientCapabilities::add_allowedTags() { auto w = List<std::string_view>(n_.append_child() << ryml::key("allowedTags")); w.init(); return w; }
void MarkdownClientCapabilities::init() { StructWrapper::init();  }
bool ChangeAnnotationsSupportOptions::groupsOnLabel() { return detail::NodeAsPrimitive<bool>(n_.find_child("groupsOnLabel")); }
bool ChangeAnnotationsSupportOptions::has_groupsOnLabel() const { return n_.has_child("groupsOnLabel"); }
void ChangeAnnotationsSupportOptions::set_groupsOnLabel(bool val) { auto c = n_.find_child("groupsOnLabel"); if (c.invalid()) { c = n_.append_child() << ryml::key("groupsOnLabel"); } detail::SetPrimitive(c, val); }
void ChangeAnnotationsSupportOptions::init() { StructWrapper::init();  }
List<SymbolKind> ClientSymbolKindOptions::valueSet() { return n_.find_child("valueSet"); }
bool ClientSymbolKindOptions::has_valueSet() const { return n_.has_child("valueSet"); }
List<SymbolKind> ClientSymbolKindOptions::add_valueSet() { auto w = List<SymbolKind>(n_.append_child() << ryml::key("valueSet")); w.init(); return w; }
void ClientSymbolKindOptions::init() { StructWrapper::init();  }
List<SymbolTag> ClientSymbolTagOptions::valueSet() { return n_.find_child("valueSet"); }
void ClientSymbolTagOptions::init() { StructWrapper::init(); List<SymbolTag>(n_.append_child() << ryml::key("valueSet")).init(); }
List<std::string_view> ClientSymbolResolveOptions::properties() { return n_.find_child("properties"); }
void ClientSymbolResolveOptions::init() { StructWrapper::init(); List<std::string_view>(n_.append_child() << ryml::key("properties")).init(); }
bool ClientCompletionItemOptions::snippetSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("snippetSupport")); }
bool ClientCompletionItemOptions::has_snippetSupport() const { return n_.has_child("snippetSupport"); }
void ClientCompletionItemOptions::set_snippetSupport(bool val) { auto c = n_.find_child("snippetSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("snippetSupport"); } detail::SetPrimitive(c, val); }
bool ClientCompletionItemOptions::commitCharactersSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("commitCharactersSupport")); }
bool ClientCompletionItemOptions::has_commitCharactersSupport() const { return n_.has_child("commitCharactersSupport"); }
void ClientCompletionItemOptions::set_commitCharactersSupport(bool val) { auto c = n_.find_child("commitCharactersSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("commitCharactersSupport"); } detail::SetPrimitive(c, val); }
List<MarkupKind> ClientCompletionItemOptions::documentationFormat() { return n_.find_child("documentationFormat"); }
bool ClientCompletionItemOptions::has_documentationFormat() const { return n_.has_child("documentationFormat"); }
List<MarkupKind> ClientCompletionItemOptions::add_documentationFormat() { auto w = List<MarkupKind>(n_.append_child() << ryml::key("documentationFormat")); w.init(); return w; }
bool ClientCompletionItemOptions::deprecatedSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("deprecatedSupport")); }
bool ClientCompletionItemOptions::has_deprecatedSupport() const { return n_.has_child("deprecatedSupport"); }
void ClientCompletionItemOptions::set_deprecatedSupport(bool val) { auto c = n_.find_child("deprecatedSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("deprecatedSupport"); } detail::SetPrimitive(c, val); }
bool ClientCompletionItemOptions::preselectSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("preselectSupport")); }
bool ClientCompletionItemOptions::has_preselectSupport() const { return n_.has_child("preselectSupport"); }
void ClientCompletionItemOptions::set_preselectSupport(bool val) { auto c = n_.find_child("preselectSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("preselectSupport"); } detail::SetPrimitive(c, val); }
CompletionItemTagOptions ClientCompletionItemOptions::tagSupport() { return n_.find_child("tagSupport"); }
bool ClientCompletionItemOptions::has_tagSupport() const { return n_.has_child("tagSupport"); }
CompletionItemTagOptions ClientCompletionItemOptions::add_tagSupport() { auto w = CompletionItemTagOptions(n_.append_child() << ryml::key("tagSupport")); w.init(); return w; }
bool ClientCompletionItemOptions::insertReplaceSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("insertReplaceSupport")); }
bool ClientCompletionItemOptions::has_insertReplaceSupport() const { return n_.has_child("insertReplaceSupport"); }
void ClientCompletionItemOptions::set_insertReplaceSupport(bool val) { auto c = n_.find_child("insertReplaceSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("insertReplaceSupport"); } detail::SetPrimitive(c, val); }
ClientCompletionItemResolveOptions ClientCompletionItemOptions::resolveSupport() { return n_.find_child("resolveSupport"); }
bool ClientCompletionItemOptions::has_resolveSupport() const { return n_.has_child("resolveSupport"); }
ClientCompletionItemResolveOptions ClientCompletionItemOptions::add_resolveSupport() { auto w = ClientCompletionItemResolveOptions(n_.append_child() << ryml::key("resolveSupport")); w.init(); return w; }
ClientCompletionItemInsertTextModeOptions ClientCompletionItemOptions::insertTextModeSupport() { return n_.find_child("insertTextModeSupport"); }
bool ClientCompletionItemOptions::has_insertTextModeSupport() const { return n_.has_child("insertTextModeSupport"); }
ClientCompletionItemInsertTextModeOptions ClientCompletionItemOptions::add_insertTextModeSupport() { auto w = ClientCompletionItemInsertTextModeOptions(n_.append_child() << ryml::key("insertTextModeSupport")); w.init(); return w; }
bool ClientCompletionItemOptions::labelDetailsSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("labelDetailsSupport")); }
bool ClientCompletionItemOptions::has_labelDetailsSupport() const { return n_.has_child("labelDetailsSupport"); }
void ClientCompletionItemOptions::set_labelDetailsSupport(bool val) { auto c = n_.find_child("labelDetailsSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("labelDetailsSupport"); } detail::SetPrimitive(c, val); }
void ClientCompletionItemOptions::init() { StructWrapper::init();  }
List<CompletionItemKind> ClientCompletionItemOptionsKind::valueSet() { return n_.find_child("valueSet"); }
bool ClientCompletionItemOptionsKind::has_valueSet() const { return n_.has_child("valueSet"); }
List<CompletionItemKind> ClientCompletionItemOptionsKind::add_valueSet() { auto w = List<CompletionItemKind>(n_.append_child() << ryml::key("valueSet")); w.init(); return w; }
void ClientCompletionItemOptionsKind::init() { StructWrapper::init();  }
List<std::string_view> CompletionListCapabilities::itemDefaults() { return n_.find_child("itemDefaults"); }
bool CompletionListCapabilities::has_itemDefaults() const { return n_.has_child("itemDefaults"); }
List<std::string_view> CompletionListCapabilities::add_itemDefaults() { auto w = List<std::string_view>(n_.append_child() << ryml::key("itemDefaults")); w.init(); return w; }
bool CompletionListCapabilities::applyKindSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("applyKindSupport")); }
bool CompletionListCapabilities::has_applyKindSupport() const { return n_.has_child("applyKindSupport"); }
void CompletionListCapabilities::set_applyKindSupport(bool val) { auto c = n_.find_child("applyKindSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("applyKindSupport"); } detail::SetPrimitive(c, val); }
void CompletionListCapabilities::init() { StructWrapper::init();  }
List<MarkupKind> ClientSignatureInformationOptions::documentationFormat() { return n_.find_child("documentationFormat"); }
bool ClientSignatureInformationOptions::has_documentationFormat() const { return n_.has_child("documentationFormat"); }
List<MarkupKind> ClientSignatureInformationOptions::add_documentationFormat() { auto w = List<MarkupKind>(n_.append_child() << ryml::key("documentationFormat")); w.init(); return w; }
ClientSignatureParameterInformationOptions ClientSignatureInformationOptions::parameterInformation() { return n_.find_child("parameterInformation"); }
bool ClientSignatureInformationOptions::has_parameterInformation() const { return n_.has_child("parameterInformation"); }
ClientSignatureParameterInformationOptions ClientSignatureInformationOptions::add_parameterInformation() { auto w = ClientSignatureParameterInformationOptions(n_.append_child() << ryml::key("parameterInformation")); w.init(); return w; }
bool ClientSignatureInformationOptions::activeParameterSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("activeParameterSupport")); }
bool ClientSignatureInformationOptions::has_activeParameterSupport() const { return n_.has_child("activeParameterSupport"); }
void ClientSignatureInformationOptions::set_activeParameterSupport(bool val) { auto c = n_.find_child("activeParameterSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("activeParameterSupport"); } detail::SetPrimitive(c, val); }
bool ClientSignatureInformationOptions::noActiveParameterSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("noActiveParameterSupport")); }
bool ClientSignatureInformationOptions::has_noActiveParameterSupport() const { return n_.has_child("noActiveParameterSupport"); }
void ClientSignatureInformationOptions::set_noActiveParameterSupport(bool val) { auto c = n_.find_child("noActiveParameterSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("noActiveParameterSupport"); } detail::SetPrimitive(c, val); }
void ClientSignatureInformationOptions::init() { StructWrapper::init();  }
ClientCodeActionKindOptions ClientCodeActionLiteralOptions::codeActionKind() { return n_.find_child("codeActionKind"); }
void ClientCodeActionLiteralOptions::init() { StructWrapper::init(); ClientCodeActionKindOptions(n_.append_child() << ryml::key("codeActionKind")).init(); }
List<std::string_view> ClientCodeActionResolveOptions::properties() { return n_.find_child("properties"); }
void ClientCodeActionResolveOptions::init() { StructWrapper::init(); List<std::string_view>(n_.append_child() << ryml::key("properties")).init(); }
List<CodeActionTag> CodeActionTagOptions::valueSet() { return n_.find_child("valueSet"); }
void CodeActionTagOptions::init() { StructWrapper::init(); List<CodeActionTag>(n_.append_child() << ryml::key("valueSet")).init(); }
List<std::string_view> ClientCodeLensResolveOptions::properties() { return n_.find_child("properties"); }
void ClientCodeLensResolveOptions::init() { StructWrapper::init(); List<std::string_view>(n_.append_child() << ryml::key("properties")).init(); }
List<FoldingRangeKind> ClientFoldingRangeKindOptions::valueSet() { return n_.find_child("valueSet"); }
bool ClientFoldingRangeKindOptions::has_valueSet() const { return n_.has_child("valueSet"); }
List<FoldingRangeKind> ClientFoldingRangeKindOptions::add_valueSet() { auto w = List<FoldingRangeKind>(n_.append_child() << ryml::key("valueSet")); w.init(); return w; }
void ClientFoldingRangeKindOptions::init() { StructWrapper::init();  }
bool ClientFoldingRangeOptions::collapsedText() { return detail::NodeAsPrimitive<bool>(n_.find_child("collapsedText")); }
bool ClientFoldingRangeOptions::has_collapsedText() const { return n_.has_child("collapsedText"); }
void ClientFoldingRangeOptions::set_collapsedText(bool val) { auto c = n_.find_child("collapsedText"); if (c.invalid()) { c = n_.append_child() << ryml::key("collapsedText"); } detail::SetPrimitive(c, val); }
void ClientFoldingRangeOptions::init() { StructWrapper::init();  }
bool DiagnosticsCapabilities::relatedInformation() { return detail::NodeAsPrimitive<bool>(n_.find_child("relatedInformation")); }
bool DiagnosticsCapabilities::has_relatedInformation() const { return n_.has_child("relatedInformation"); }
void DiagnosticsCapabilities::set_relatedInformation(bool val) { auto c = n_.find_child("relatedInformation"); if (c.invalid()) { c = n_.append_child() << ryml::key("relatedInformation"); } detail::SetPrimitive(c, val); }
ClientDiagnosticsTagOptions DiagnosticsCapabilities::tagSupport() { return n_.find_child("tagSupport"); }
bool DiagnosticsCapabilities::has_tagSupport() const { return n_.has_child("tagSupport"); }
ClientDiagnosticsTagOptions DiagnosticsCapabilities::add_tagSupport() { auto w = ClientDiagnosticsTagOptions(n_.append_child() << ryml::key("tagSupport")); w.init(); return w; }
bool DiagnosticsCapabilities::codeDescriptionSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("codeDescriptionSupport")); }
bool DiagnosticsCapabilities::has_codeDescriptionSupport() const { return n_.has_child("codeDescriptionSupport"); }
void DiagnosticsCapabilities::set_codeDescriptionSupport(bool val) { auto c = n_.find_child("codeDescriptionSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("codeDescriptionSupport"); } detail::SetPrimitive(c, val); }
bool DiagnosticsCapabilities::dataSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("dataSupport")); }
bool DiagnosticsCapabilities::has_dataSupport() const { return n_.has_child("dataSupport"); }
void DiagnosticsCapabilities::set_dataSupport(bool val) { auto c = n_.find_child("dataSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("dataSupport"); } detail::SetPrimitive(c, val); }
void DiagnosticsCapabilities::init() { StructWrapper::init();  }
bool ClientSemanticTokensRequestOptions::Union0::holds_boolean() const { return n_.has_child("TODO"); }
bool ClientSemanticTokensRequestOptions::Union0::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ClientSemanticTokensRequestOptions::Union0::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
ClientSemanticTokensRequestOptions::Union0 ClientSemanticTokensRequestOptions::range() { return n_.find_child("range"); }
bool ClientSemanticTokensRequestOptions::has_range() const { return n_.has_child("range"); }
ClientSemanticTokensRequestOptions::Union0 ClientSemanticTokensRequestOptions::add_range() { auto w = ClientSemanticTokensRequestOptions::Union0(n_.append_child() << ryml::key("range")); w.init(); return w; }
bool ClientSemanticTokensRequestOptions::Union1::holds_boolean() const { return n_.has_child("TODO"); }
bool ClientSemanticTokensRequestOptions::Union1::as_boolean() { return detail::NodeAsPrimitive<bool>(n_); }
void ClientSemanticTokensRequestOptions::Union1::set_boolean(bool val) { detail::SetPrimitive(n_, val); }
bool ClientSemanticTokensRequestOptions::Union1::holds_ClientSemanticTokensRequestFullDelta() const { return n_.has_child("TODO"); }
ClientSemanticTokensRequestFullDelta ClientSemanticTokensRequestOptions::Union1::as_ClientSemanticTokensRequestFullDelta() { return {n_}; }
ClientSemanticTokensRequestOptions::Union1 ClientSemanticTokensRequestOptions::full() { return n_.find_child("full"); }
bool ClientSemanticTokensRequestOptions::has_full() const { return n_.has_child("full"); }
ClientSemanticTokensRequestOptions::Union1 ClientSemanticTokensRequestOptions::add_full() { auto w = ClientSemanticTokensRequestOptions::Union1(n_.append_child() << ryml::key("full")); w.init(); return w; }
void ClientSemanticTokensRequestOptions::init() { StructWrapper::init();  }
List<std::string_view> ClientInlayHintResolveOptions::properties() { return n_.find_child("properties"); }
void ClientInlayHintResolveOptions::init() { StructWrapper::init(); List<std::string_view>(n_.append_child() << ryml::key("properties")).init(); }
bool ClientShowMessageActionItemOptions::additionalPropertiesSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("additionalPropertiesSupport")); }
bool ClientShowMessageActionItemOptions::has_additionalPropertiesSupport() const { return n_.has_child("additionalPropertiesSupport"); }
void ClientShowMessageActionItemOptions::set_additionalPropertiesSupport(bool val) { auto c = n_.find_child("additionalPropertiesSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("additionalPropertiesSupport"); } detail::SetPrimitive(c, val); }
void ClientShowMessageActionItemOptions::init() { StructWrapper::init();  }
List<CompletionItemTag> CompletionItemTagOptions::valueSet() { return n_.find_child("valueSet"); }
void CompletionItemTagOptions::init() { StructWrapper::init(); List<CompletionItemTag>(n_.append_child() << ryml::key("valueSet")).init(); }
List<std::string_view> ClientCompletionItemResolveOptions::properties() { return n_.find_child("properties"); }
void ClientCompletionItemResolveOptions::init() { StructWrapper::init(); List<std::string_view>(n_.append_child() << ryml::key("properties")).init(); }
List<InsertTextMode> ClientCompletionItemInsertTextModeOptions::valueSet() { return n_.find_child("valueSet"); }
void ClientCompletionItemInsertTextModeOptions::init() { StructWrapper::init(); List<InsertTextMode>(n_.append_child() << ryml::key("valueSet")).init(); }
bool ClientSignatureParameterInformationOptions::labelOffsetSupport() { return detail::NodeAsPrimitive<bool>(n_.find_child("labelOffsetSupport")); }
bool ClientSignatureParameterInformationOptions::has_labelOffsetSupport() const { return n_.has_child("labelOffsetSupport"); }
void ClientSignatureParameterInformationOptions::set_labelOffsetSupport(bool val) { auto c = n_.find_child("labelOffsetSupport"); if (c.invalid()) { c = n_.append_child() << ryml::key("labelOffsetSupport"); } detail::SetPrimitive(c, val); }
void ClientSignatureParameterInformationOptions::init() { StructWrapper::init();  }
List<CodeActionKind> ClientCodeActionKindOptions::valueSet() { return n_.find_child("valueSet"); }
void ClientCodeActionKindOptions::init() { StructWrapper::init(); List<CodeActionKind>(n_.append_child() << ryml::key("valueSet")).init(); }
List<DiagnosticTag> ClientDiagnosticsTagOptions::valueSet() { return n_.find_child("valueSet"); }
void ClientDiagnosticsTagOptions::init() { StructWrapper::init(); List<DiagnosticTag>(n_.append_child() << ryml::key("valueSet")).init(); }
bool ClientSemanticTokensRequestFullDelta::delta() { return detail::NodeAsPrimitive<bool>(n_.find_child("delta")); }
bool ClientSemanticTokensRequestFullDelta::has_delta() const { return n_.has_child("delta"); }
void ClientSemanticTokensRequestFullDelta::set_delta(bool val) { auto c = n_.find_child("delta"); if (c.invalid()) { c = n_.append_child() << ryml::key("delta"); } detail::SetPrimitive(c, val); }
void ClientSemanticTokensRequestFullDelta::init() { StructWrapper::init();  }

}  // namespace lsp
