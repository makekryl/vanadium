#pragma once

#include <span>
#include <cstdint>
#include <tuple>
#include <string_view>
#include <optional>
#include <variant>
#include <ryml.hpp>

#include "LSProtocolBase.h"

// NOLINTBEGIN(readability-identifier-naming)

namespace lsp {

/**
 * A set of predefined token types. This set is not fixed
 * an clients can specify additional token types via the
 * corresponding client capabilities.
 * 
 * @since 3.16.0
 */
enum class SemanticTokenTypes {
  kNamespace,

  // Represents a generic type. Acts as a fallback for types which can't be mapped to
  // a specific type like class or enum.
  kType,
  kClass,
  kEnum,
  kInterface,
  kStruct,
  kTypeparameter,
  kParameter,
  kVariable,
  kProperty,
  kEnummember,
  kEvent,
  kFunction,
  kMethod,
  kMacro,
  kKeyword,
  kModifier,
  kComment,
  kString,
  kNumber,
  kRegexp,
  kOperator,

  // @since 3.17.0
  kDecorator,

  // @since 3.18.0
  kLabel,
};

/**
 * A set of predefined token modifiers. This set is not fixed
 * an clients can specify additional token types via the
 * corresponding client capabilities.
 * 
 * @since 3.16.0
 */
enum class SemanticTokenModifiers {
  kDeclaration,
  kDefinition,
  kReadonly,
  kStatic,
  kDeprecated,
  kAbstract,
  kAsync,
  kModification,
  kDocumentation,
  kDefaultlibrary,
};

/**
 * The document diagnostic report kinds.
 * 
 * @since 3.17.0
 */
enum class DocumentDiagnosticReportKind {
  // A diagnostic report with a full
  // set of problems.
  kFull,

  // A report indicating that the last
  // returned report is still accurate.
  kUnchanged,
};

/**
 * Predefined error codes.
 */
enum class ErrorCodes {
  kParseerror = -32700,
  kInvalidrequest = -32600,
  kMethodnotfound = -32601,
  kInvalidparams = -32602,
  kInternalerror = -32603,

  // Error code indicating that a server received a notification or
  // request before the server has received the `initialize` request.
  kServernotinitialized = -32002,
  kUnknownerrorcode = -32001,
};

enum class LSPErrorCodes {
  // A request failed but it was syntactically correct, e.g the
  // method name was known and the parameters were valid. The error
  // message should contain human readable information about why
  // the request failed.
  // 
  // @since 3.17.0
  kRequestfailed = -32803,

  // The server cancelled the request. This error code should
  // only be used for requests that explicitly support being
  // server cancellable.
  // 
  // @since 3.17.0
  kServercancelled = -32802,

  // The server detected that the content of a document got
  // modified outside normal conditions. A server should
  // NOT send this error code if it detects a content change
  // in it unprocessed messages. The result even computed
  // on an older state might still be useful for the client.
  // 
  // If a client decides that a result is not of any use anymore
  // the client should cancel the request.
  kContentmodified = -32801,

  // The client has canceled a request and a server has detected
  // the cancel.
  kRequestcancelled = -32800,
};

/**
 * A set of predefined range kinds.
 */
enum class FoldingRangeKind {
  // Folding range for a comment
  kComment,

  // Folding range for an import or include
  kImports,

  // Folding range for a region (e.g. `#region`)
  kRegion,
};

/**
 * A symbol kind.
 */
enum class SymbolKind {
  kFile = 1,
  kModule = 2,
  kNamespace = 3,
  kPackage = 4,
  kClass = 5,
  kMethod = 6,
  kProperty = 7,
  kField = 8,
  kConstructor = 9,
  kEnum = 10,
  kInterface = 11,
  kFunction = 12,
  kVariable = 13,
  kConstant = 14,
  kString = 15,
  kNumber = 16,
  kBoolean = 17,
  kArray = 18,
  kObject = 19,
  kKey = 20,
  kNull = 21,
  kEnummember = 22,
  kStruct = 23,
  kEvent = 24,
  kOperator = 25,
  kTypeparameter = 26,
};

/**
 * Symbol tags are extra annotations that tweak the rendering of a symbol.
 * 
 * @since 3.16
 */
enum class SymbolTag {
  // Render a symbol as obsolete, usually using a strike-out.
  kDeprecated = 1,
};

/**
 * Moniker uniqueness level to define scope of the moniker.
 * 
 * @since 3.16.0
 */
enum class UniquenessLevel {
  // The moniker is only unique inside a document
  kDocument,

  // The moniker is unique inside a project for which a dump got created
  kProject,

  // The moniker is unique inside the group to which a project belongs
  kGroup,

  // The moniker is unique inside the moniker scheme.
  kScheme,

  // The moniker is globally unique
  kGlobal,
};

/**
 * The moniker kind.
 * 
 * @since 3.16.0
 */
enum class MonikerKind {
  // The moniker represent a symbol that is imported into a project
  kImport,

  // The moniker represents a symbol that is exported from a project
  kExport,

  // The moniker represents a symbol that is local to a project (e.g. a local
  // variable of a function, a class not visible outside the project, ...)
  kLocal,
};

/**
 * Inlay hint kinds.
 * 
 * @since 3.17.0
 */
enum class InlayHintKind {
  // An inlay hint that for a type annotation.
  kType = 1,

  // An inlay hint that is for a parameter.
  kParameter = 2,
};

/**
 * The message type
 */
enum class MessageType {
  // An error message.
  kError = 1,

  // A warning message.
  kWarning = 2,

  // An information message.
  kInfo = 3,

  // A log message.
  kLog = 4,

  // A debug message.
  // 
  // @since 3.18.0
  // @proposed
  kDebug = 5,
};

/**
 * Defines how the host (editor) should sync
 * document changes to the language server.
 */
enum class TextDocumentSyncKind {
  // Documents should not be synced at all.
  kNone = 0,

  // Documents are synced by always sending the full content
  // of the document.
  kFull = 1,

  // Documents are synced by sending the full content on open.
  // After that only incremental updates to the document are
  // send.
  kIncremental = 2,
};

/**
 * Represents reasons why a text document is saved.
 */
enum class TextDocumentSaveReason {
  // Manually triggered, e.g. by the user pressing save, by starting debugging,
  // or by an API call.
  kManual = 1,

  // Automatic after a delay.
  kAfterdelay = 2,

  // When the editor lost focus.
  kFocusout = 3,
};

/**
 * The kind of a completion entry.
 */
enum class CompletionItemKind {
  kText = 1,
  kMethod = 2,
  kFunction = 3,
  kConstructor = 4,
  kField = 5,
  kVariable = 6,
  kClass = 7,
  kInterface = 8,
  kModule = 9,
  kProperty = 10,
  kUnit = 11,
  kValue = 12,
  kEnum = 13,
  kKeyword = 14,
  kSnippet = 15,
  kColor = 16,
  kFile = 17,
  kReference = 18,
  kFolder = 19,
  kEnummember = 20,
  kConstant = 21,
  kStruct = 22,
  kEvent = 23,
  kOperator = 24,
  kTypeparameter = 25,
};

/**
 * Completion item tags are extra annotations that tweak the rendering of a completion
 * item.
 * 
 * @since 3.15.0
 */
enum class CompletionItemTag {
  // Render a completion as obsolete, usually using a strike-out.
  kDeprecated = 1,
};

/**
 * Defines whether the insert text in a completion item should be interpreted as
 * plain text or a snippet.
 */
enum class InsertTextFormat {
  // The primary text to be inserted is treated as a plain string.
  kPlaintext = 1,

  // The primary text to be inserted is treated as a snippet.
  // 
  // A snippet can define tab stops and placeholders with `$1`, `$2`
  // and `${3:foo}`. `$0` defines the final tab stop, it defaults to
  // the end of the snippet. Placeholders with equal identifiers are linked,
  // that is typing in one will update others too.
  // 
  // See also: https://microsoft.github.io/language-server-protocol/specifications/specification-current/#snippet_syntax
  kSnippet = 2,
};

/**
 * How whitespace and indentation is handled during completion
 * item insertion.
 * 
 * @since 3.16.0
 */
enum class InsertTextMode {
  // The insertion or replace strings is taken as it is. If the
  // value is multi line the lines below the cursor will be
  // inserted using the indentation defined in the string value.
  // The client will not apply any kind of adjustments to the
  // string.
  kAsis = 1,

  // The editor adjusts leading whitespace of new lines so that
  // they match the indentation up to the cursor of the line for
  // which the item is accepted.
  // 
  // Consider a line like this: <2tabs><cursor><3tabs>foo. Accepting a
  // multi line completion item is indented using 2 tabs and all
  // following lines inserted will be indented using 2 tabs as well.
  kAdjustindentation = 2,
};

/**
 * A document highlight kind.
 */
enum class DocumentHighlightKind {
  // A textual occurrence.
  kText = 1,

  // Read-access of a symbol, like reading a variable.
  kRead = 2,

  // Write-access of a symbol, like writing to a variable.
  kWrite = 3,
};

/**
 * A set of predefined code action kinds
 */
enum class CodeActionKind {
  // Empty kind.
  kEmpty,

  // Base kind for quickfix actions: 'quickfix'
  kQuickfix,

  // Base kind for refactoring actions: 'refactor'
  kRefactor,

  // Base kind for refactoring extraction actions: 'refactor.extract'
  // 
  // Example extract actions:
  // 
  // - Extract method
  // - Extract function
  // - Extract variable
  // - Extract interface from class
  // - ...
  kRefactorextract,

  // Base kind for refactoring inline actions: 'refactor.inline'
  // 
  // Example inline actions:
  // 
  // - Inline function
  // - Inline variable
  // - Inline constant
  // - ...
  kRefactorinline,

  // Base kind for refactoring move actions: `refactor.move`
  // 
  // Example move actions:
  // 
  // - Move a function to a new file
  // - Move a property between classes
  // - Move method to base class
  // - ...
  // 
  // @since 3.18.0
  // @proposed
  kRefactormove,

  // Base kind for refactoring rewrite actions: 'refactor.rewrite'
  // 
  // Example rewrite actions:
  // 
  // - Convert JavaScript function to class
  // - Add or remove parameter
  // - Encapsulate field
  // - Make method static
  // - Move method to base class
  // - ...
  kRefactorrewrite,

  // Base kind for source actions: `source`
  // 
  // Source code actions apply to the entire file.
  kSource,

  // Base kind for an organize imports source action: `source.organizeImports`
  kSourceorganizeimports,

  // Base kind for auto-fix source actions: `source.fixAll`.
  // 
  // Fix all actions automatically fix errors that have a clear fix that do not require user input.
  // They should not suppress errors or perform unsafe fixes such as generating new types or classes.
  // 
  // @since 3.15.0
  kSourcefixall,

  // Base kind for all code actions applying to the entire notebook's scope. CodeActionKinds using
  // this should always begin with `notebook.`
  // 
  // @since 3.18.0
  kNotebook,
};

/**
 * Code action tags are extra annotations that tweak the behavior of a code action.
 * 
 * @since 3.18.0 - proposed
 */
enum class CodeActionTag {
  // Marks the code action as LLM-generated.
  kLlmgenerated = 1,
};

enum class TraceValue {
  // Turn tracing off.
  kOff,

  // Trace messages only.
  kMessages,

  // Verbose message tracing.
  kVerbose,
};

/**
 * Describes the content type that a client supports in various
 * result literals like `Hover`, `ParameterInfo` or `CompletionItem`.
 * 
 * Please note that `MarkupKinds` must not start with a `$`. This kinds
 * are reserved for internal usage.
 */
enum class MarkupKind {
  // Plain text is supported as a content format
  kPlaintext,

  // Markdown is supported as a content format
  kMarkdown,
};

/**
 * Predefined Language kinds
 * @since 3.18.0
 */
enum class LanguageKind {
  kAbap,
  kWindowsbat,
  kBibtex,
  kClojure,
  kCoffeescript,
  kC,
  kCpp,
  kCsharp,
  kCss,

  // @since 3.18.0
  // @proposed
  kD,

  // @since 3.18.0
  // @proposed
  kDelphi,
  kDiff,
  kDart,
  kDockerfile,
  kElixir,
  kErlang,
  kFsharp,
  kGitcommit,
  kGitrebase,
  kGo,
  kGroovy,
  kHandlebars,
  kHaskell,
  kHtml,
  kIni,
  kJava,
  kJavascript,
  kJavascriptreact,
  kJson,
  kLatex,
  kLess,
  kLua,
  kMakefile,
  kMarkdown,
  kObjectivec,
  kObjectivecpp,

  // @since 3.18.0
  // @proposed
  kPascal,
  kPerl,
  kPerl6,
  kPhp,
  kPowershell,
  kPug,
  kPython,
  kR,
  kRazor,
  kRuby,
  kRust,
  kScss,
  kSass,
  kScala,
  kShaderlab,
  kShellscript,
  kSql,
  kSwift,
  kTypescript,
  kTypescriptreact,
  kTex,
  kVisualbasic,
  kXml,
  kXsl,
  kYaml,
};

/**
 * Describes how an {@link InlineCompletionItemProvider inline completion provider} was triggered.
 * 
 * @since 3.18.0
 * @proposed
 */
enum class InlineCompletionTriggerKind {
  // Completion was triggered explicitly by a user gesture.
  kInvoked = 1,

  // Completion was triggered automatically while editing.
  kAutomatic = 2,
};

/**
 * A set of predefined position encoding kinds.
 * 
 * @since 3.17.0
 */
enum class PositionEncodingKind {
  // Character offsets count UTF-8 code units (e.g. bytes).
  kUtf8,

  // Character offsets count UTF-16 code units.
  // 
  // This is the default and must always be supported
  // by servers
  kUtf16,

  // Character offsets count UTF-32 code units.
  // 
  // Implementation note: these are the same as Unicode codepoints,
  // so this `PositionEncodingKind` may also be used for an
  // encoding-agnostic representation of character offsets.
  kUtf32,
};

/**
 * The file event type
 */
enum class FileChangeType {
  // The file got created.
  kCreated = 1,

  // The file got changed.
  kChanged = 2,

  // The file got deleted.
  kDeleted = 3,
};

enum class WatchKind {
  // Interested in create events.
  kCreate = 1,

  // Interested in change events
  kChange = 2,

  // Interested in delete events
  kDelete = 4,
};

/**
 * The diagnostic's severity.
 */
enum class DiagnosticSeverity {
  // Reports an error.
  kError = 1,

  // Reports a warning.
  kWarning = 2,

  // Reports an information.
  kInformation = 3,

  // Reports a hint.
  kHint = 4,
};

/**
 * The diagnostic tags.
 * 
 * @since 3.15.0
 */
enum class DiagnosticTag {
  // Unused or unnecessary code.
  // 
  // Clients are allowed to render diagnostics with this tag faded out instead of having
  // an error squiggle.
  kUnnecessary = 1,

  // Deprecated or obsolete code.
  // 
  // Clients are allowed to rendered diagnostics with this tag strike through.
  kDeprecated = 2,
};

/**
 * How a completion was triggered
 */
enum class CompletionTriggerKind {
  // Completion was triggered by typing an identifier (24x7 code
  // complete), manual invocation (e.g Ctrl+Space) or via API.
  kInvoked = 1,

  // Completion was triggered by a trigger character specified by
  // the `triggerCharacters` properties of the `CompletionRegistrationOptions`.
  kTriggercharacter = 2,

  // Completion was re-triggered as current completion list is incomplete
  kTriggerforincompletecompletions = 3,
};

/**
 * Defines how values from a set of defaults and an individual item will be
 * merged.
 * 
 * @since 3.18.0
 */
enum class ApplyKind {
  // The value from the individual item (if provided and not `null`) will be
  // used instead of the default.
  kReplace = 1,

  // The value from the item will be merged with the default.
  // 
  // The specific rules for mergeing values are defined against each field
  // that supports merging.
  kMerge = 2,
};

/**
 * How a signature help was triggered.
 * 
 * @since 3.15.0
 */
enum class SignatureHelpTriggerKind {
  // Signature help was invoked manually by the user or by a command.
  kInvoked = 1,

  // Signature help was triggered by a trigger character.
  kTriggercharacter = 2,

  // Signature help was triggered by the cursor moving or by the document content changing.
  kContentchange = 3,
};

/**
 * The reason why code actions were requested.
 * 
 * @since 3.17.0
 */
enum class CodeActionTriggerKind {
  // Code actions were explicitly requested by the user or by an extension.
  kInvoked = 1,

  // Code actions were requested automatically.
  // 
  // This typically happens when current selection in a file changes, but can
  // also be triggered when file content changes.
  kAutomatic = 2,
};

/**
 * A pattern kind describing if a glob pattern matches a file a folder or
 * both.
 * 
 * @since 3.16.0
 */
enum class FileOperationPatternKind {
  // The pattern matches a file only.
  kFile,

  // The pattern matches a folder only.
  kFolder,
};

/**
 * A notebook cell kind.
 * 
 * @since 3.17.0
 */
enum class NotebookCellKind {
  // A markup-cell is formatted source that is used for display.
  kMarkup = 1,

  // A code-cell is source code.
  kCode = 2,
};

enum class ResourceOperationKind {
  // Supports creating new files and folders.
  kCreate,

  // Supports renaming existing files and folders.
  kRename,

  // Supports deleting existing files and folders.
  kDelete,
};

enum class FailureHandlingKind {
  // Applying the workspace change is simply aborted if one of the changes provided
  // fails. All operations executed before the failing operation stay executed.
  kAbort,

  // All operations are executed transactional. That means they either all
  // succeed or no changes at all are applied to the workspace.
  kTransactional,

  // If the workspace edit contains only textual file changes they are executed transactional.
  // If resource changes (create, rename or delete file) are part of the change the failure
  // handling strategy is abort.
  kTextonlytransactional,

  // The client tries to undo the operations already executed. But there is no
  // guarantee that this is succeeding.
  kUndo,
};

enum class PrepareSupportDefaultBehavior {
  // The client's default behavior is to select the identifier
  // according the to language's syntax rule.
  kIdentifier = 1,
};

enum class TokenFormat {
  kRelative,
};

/**
 * @since 3.18.0
 */
struct PrepareRenameDefaultBehavior : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool defaultBehavior();
  void set_defaultBehavior(bool val);

  void init();
};

struct ProgressToken : detail::StructWrapper {
  using StructWrapper::StructWrapper;

};

/**
 * An identifier to refer to a change annotation stored with a workspace edit.
 */
using ChangeAnnotationIdentifier = std::string_view;

/**
 * @since 3.18.0
 */
struct TextDocumentContentChangeWholeDocument : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The new text of the whole document.
  [[nodiscard]] std::string_view text();
  void set_text(std::string_view val);

  void init();
};

/**
 * @since 3.18.0
 * @deprecated use MarkupContent instead.
 */
struct MarkedStringWithLanguage : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] std::string_view language();
  void set_language(std::string_view val);
  [[nodiscard]] std::string_view value();
  void set_value(std::string_view val);

  void init();
};

/**
 * The glob pattern to watch relative to the base path. Glob patterns can have the following syntax:
 * - `*` to match one or more characters in a path segment
 * - `?` to match on one character in a path segment
 * - `**` to match any number of path segments, including none
 * - `{}` to group conditions (e.g. `**​/\*.{ts,js}` matches all TypeScript and JavaScript files)
 * - `[]` to declare a range of characters to match in a path segment (e.g., `example.[0-9]` to match on `example.0`, `example.1`, …)
 * - `[!...]` to negate a range of characters to match in a path segment (e.g., `example.[!0-9]` to match on `example.a`, `example.b`, but not `example.0`)
 * 
 * @since 3.17.0
 */
using Pattern = std::string_view;

using RegularExpressionEngineKind = std::string_view;

/**
 * Static registration options to be returned in the initialize
 * request.
 */
struct StaticRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

/**
 * A workspace folder inside a client.
 */
struct WorkspaceFolder : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The associated URI for this workspace folder.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  // The name of the workspace folder. Used to refer to this
  // workspace folder in the user interface.
  [[nodiscard]] std::string_view name();
  void set_name(std::string_view val);

  void init();
};

struct ConfigurationItem : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The scope to get the configuration section for.
  [[nodiscard]] std::string_view scopeUri();
  [[nodiscard]] bool has_scopeUri() const;
  void set_scopeUri(std::string_view val);

  // The configuration section asked for.
  [[nodiscard]] std::string_view section();
  [[nodiscard]] bool has_section() const;
  void set_section(std::string_view val);

  void init();
};

/**
 * A literal to identify a text document in the client.
 */
struct TextDocumentIdentifier : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document's uri.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  void init();
};

/**
 * Represents a color in RGBA space.
 */
struct Color : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The red component of this color in the range [0-1].
  [[nodiscard]] double red();
  void set_red(double val);

  // The green component of this color in the range [0-1].
  [[nodiscard]] double green();
  void set_green(double val);

  // The blue component of this color in the range [0-1].
  [[nodiscard]] double blue();
  void set_blue(double val);

  // The alpha component of this color in the range [0-1].
  [[nodiscard]] double alpha();
  void set_alpha(double val);

  void init();
};

struct WorkDoneProgressOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Position in a text document expressed as zero-based line and character
 * offset. Prior to 3.17 the offsets were always based on a UTF-16 string
 * representation. So a string of the form `a𐐀b` the character offset of the
 * character `a` is 0, the character offset of `𐐀` is 1 and the character
 * offset of b is 3 since `𐐀` is represented using two code units in UTF-16.
 * Since 3.17 clients and servers can agree on a different string encoding
 * representation (e.g. UTF-8). The client announces it's supported encoding
 * via the client capability [`general.positionEncodings`](https://microsoft.github.io/language-server-protocol/specifications/specification-current/#clientCapabilities).
 * The value is an array of position encodings the client supports, with
 * decreasing preference (e.g. the encoding at index `0` is the most preferred
 * one). To stay backwards compatible the only mandatory encoding is UTF-16
 * represented via the string `utf-16`. The server can pick one of the
 * encodings offered by the client and signals that encoding back to the
 * client via the initialize result's property
 * [`capabilities.positionEncoding`](https://microsoft.github.io/language-server-protocol/specifications/specification-current/#serverCapabilities). If the string value
 * `utf-16` is missing from the client's capability `general.positionEncodings`
 * servers can safely assume that the client supports UTF-16. If the server
 * omits the position encoding in its initialize result the encoding defaults
 * to the string value `utf-16`. Implementation considerations: since the
 * conversion from one encoding into another requires the content of the
 * file / line the conversion is best done where the file is read which is
 * usually on the server side.
 * 
 * Positions are line end character agnostic. So you can not specify a position
 * that denotes `\r|\n` or `\n|` where `|` represents the character offset.
 * 
 * @since 3.17.0 - support for negotiated position encoding.
 */
struct Position : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Line position in a document (zero-based).
  [[nodiscard]] std::uint32_t line();
  void set_line(std::uint32_t val);

  // Character offset on a line in a document (zero-based).
  // 
  // The meaning of this offset is determined by the negotiated
  // `PositionEncodingKind`.
  [[nodiscard]] std::uint32_t character();
  void set_character(std::uint32_t val);

  void init();
};

/**
 * @since 3.16.0
 */
struct SemanticTokens : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // An optional result id. If provided and clients support delta updating
  // the client will include the result id in the next semantic token request.
  // A server can then instead of computing all semantic tokens again simply
  // send a delta.
  [[nodiscard]] std::string_view resultId();
  [[nodiscard]] bool has_resultId() const;
  void set_resultId(std::string_view val);

  // The actual tokens.
  [[nodiscard]] List<std::uint32_t> data();

  void init();
};

/**
 * @since 3.16.0
 */
struct SemanticTokensPartialResult : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] List<std::uint32_t> data();

  void init();
};

/**
 * @since 3.16.0
 */
struct SemanticTokensEdit : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The start offset of the edit.
  [[nodiscard]] std::uint32_t start();
  void set_start(std::uint32_t val);

  // The count of elements to remove.
  [[nodiscard]] std::uint32_t deleteCount();
  void set_deleteCount(std::uint32_t val);

  // The elements to insert.
  [[nodiscard]] List<std::uint32_t> data();
  [[nodiscard]] bool has_data() const;
  [[nodiscard]] List<std::uint32_t> add_data();

  void init();
};

/**
 * The result of a showDocument request.
 * 
 * @since 3.16.0
 */
struct ShowDocumentResult : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A boolean indicating if the show was successful.
  [[nodiscard]] bool success();
  void set_success(bool val);

  void init();
};

/**
 * Represents information on a file/folder create.
 * 
 * @since 3.16.0
 */
struct FileCreate : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A file:// URI for the location of the file/folder being created.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  void init();
};

/**
 * Additional information that describes document changes.
 * 
 * @since 3.16.0
 */
struct ChangeAnnotation : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A human-readable string describing the actual change. The string
  // is rendered prominent in the user interface.
  [[nodiscard]] std::string_view label();
  void set_label(std::string_view val);

  // A flag which indicates that user confirmation is needed
  // before applying the change.
  [[nodiscard]] bool needsConfirmation();
  [[nodiscard]] bool has_needsConfirmation() const;
  void set_needsConfirmation(bool val);

  // A human-readable string which is rendered less prominent in
  // the user interface.
  [[nodiscard]] std::string_view description();
  [[nodiscard]] bool has_description() const;
  void set_description(std::string_view val);

  void init();
};

/**
 * Represents information on a file/folder rename.
 * 
 * @since 3.16.0
 */
struct FileRename : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A file:// URI for the original location of the file/folder being renamed.
  [[nodiscard]] std::string_view oldUri();
  void set_oldUri(std::string_view val);

  // A file:// URI for the new location of the file/folder being renamed.
  [[nodiscard]] std::string_view newUri();
  void set_newUri(std::string_view val);

  void init();
};

/**
 * Represents information on a file/folder delete.
 * 
 * @since 3.16.0
 */
struct FileDelete : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A file:// URI for the location of the file/folder being deleted.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  void init();
};

/**
 * A diagnostic report indicating that the last returned
 * report is still accurate.
 * 
 * @since 3.17.0
 */
struct UnchangedDocumentDiagnosticReport : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A result id which will be sent on the next
  // diagnostic request for the same document.
  [[nodiscard]] std::string_view resultId();
  void set_resultId(std::string_view val);

  void init();
};

/**
 * Cancellation data returned from a diagnostic request.
 * 
 * @since 3.17.0
 */
struct DiagnosticServerCancellationData : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool retriggerRequest();
  void set_retriggerRequest(bool val);

  void init();
};

/**
 * A previous result id in a workspace pull request.
 * 
 * @since 3.17.0
 */
struct PreviousResultId : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The URI for which the client knowns a
  // result id.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  // The value of the previous result id.
  [[nodiscard]] std::string_view value();
  void set_value(std::string_view val);

  void init();
};

/**
 * A versioned notebook document identifier.
 * 
 * @since 3.17.0
 */
struct VersionedNotebookDocumentIdentifier : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The version number of this notebook document.
  [[nodiscard]] std::int32_t version();
  void set_version(std::int32_t val);

  // The notebook document's uri.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  void init();
};

/**
 * A literal to identify a notebook document in the client.
 * 
 * @since 3.17.0
 */
struct NotebookDocumentIdentifier : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The notebook document's uri.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  void init();
};

/**
 * A string value used as a snippet is a template which allows to insert text
 * and to control the editor cursor when insertion happens.
 * 
 * A snippet can define tab stops and placeholders with `$1`, `$2`
 * and `${3:foo}`. `$0` defines the final tab stop, it defaults to
 * the end of the snippet. Variables are defined with `$name` and
 * `${name:default value}`.
 * 
 * @since 3.18.0
 * @proposed
 */
struct StringValue : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The snippet string.
  [[nodiscard]] std::string_view value();
  void set_value(std::string_view val);

  void init();
};

/**
 * Parameters for the `workspace/textDocumentContent` request.
 * 
 * @since 3.18.0
 * @proposed
 */
struct TextDocumentContentParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The uri of the text document.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  void init();
};

/**
 * Result of the `workspace/textDocumentContent` request.
 * 
 * @since 3.18.0
 * @proposed
 */
struct TextDocumentContentResult : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text content of the text document. Please note, that the content of
  // any subsequent open notifications for the text document might differ
  // from the returned content due to whitespace and line ending
  // normalizations done on the client
  [[nodiscard]] std::string_view text();
  void set_text(std::string_view val);

  void init();
};

/**
 * Text document content provider options.
 * 
 * @since 3.18.0
 * @proposed
 */
struct TextDocumentContentOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The schemes for which the server provides content.
  [[nodiscard]] List<std::string_view> schemes();

  void init();
};

/**
 * Parameters for the `workspace/textDocumentContent/refresh` request.
 * 
 * @since 3.18.0
 * @proposed
 */
struct TextDocumentContentRefreshParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The uri of the text document to refresh.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  void init();
};

/**
 * General parameters to unregister a request or notification.
 */
struct Unregistration : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The id used to unregister the request or notification. Usually an id
  // provided during the register request.
  [[nodiscard]] std::string_view id();
  void set_id(std::string_view val);

  // The method to unregister for.
  [[nodiscard]] std::string_view method();
  void set_method(std::string_view val);

  void init();
};

/**
 * Information about the server
 * 
 * @since 3.15.0
 * @since 3.18.0 ServerInfo type name added.
 */
struct ServerInfo : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The name of the server as defined by the server.
  [[nodiscard]] std::string_view name();
  void set_name(std::string_view val);

  // The server's version as defined by the server.
  [[nodiscard]] std::string_view version();
  [[nodiscard]] bool has_version() const;
  void set_version(std::string_view val);

  void init();
};

/**
 * The data type of the ResponseError if the
 * initialize request fails.
 */
struct InitializeError : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Indicates whether the client execute the following retry logic:
  // (1) show the message provided by the ResponseError to the user
  // (2) user selects retry or cancel
  // (3) if user selected retry the initialize method is sent again.
  [[nodiscard]] bool retry();
  void set_retry(bool val);

  void init();
};

struct InitializedParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;


  void init();
};

struct DidChangeConfigurationRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  struct Union0 : detail::UnionStructWrapper<std::string_view, List<std::string_view>> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_array_string() const;
    [[nodiscard]] List<std::string_view> as_array_string();

  };

  [[nodiscard]] DidChangeConfigurationRegistrationOptions::Union0 section();
  [[nodiscard]] bool has_section() const;
  [[nodiscard]] DidChangeConfigurationRegistrationOptions::Union0 add_section();

  void init();
};

struct MessageActionItem : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A short title like 'Retry', 'Open Log' etc.
  [[nodiscard]] std::string_view title();
  void set_title(std::string_view val);

  void init();
};

/**
 * Save options.
 */
struct SaveOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The client is supposed to include the content on save.
  [[nodiscard]] bool includeText();
  [[nodiscard]] bool has_includeText() const;
  void set_includeText(bool val);

  void init();
};

/**
 * Additional details for a completion item label.
 * 
 * @since 3.17.0
 */
struct CompletionItemLabelDetails : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // An optional string which is rendered less prominently directly after {@link CompletionItem.label label},
  // without any spacing. Should be used for function signatures and type annotations.
  [[nodiscard]] std::string_view detail();
  [[nodiscard]] bool has_detail() const;
  void set_detail(std::string_view val);

  // An optional string which is rendered less prominently after {@link CompletionItem.detail}. Should be used
  // for fully qualified names and file paths.
  [[nodiscard]] std::string_view description();
  [[nodiscard]] bool has_description() const;
  void set_description(std::string_view val);

  void init();
};

/**
 * Value-object that contains additional information when
 * requesting references.
 */
struct ReferenceContext : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Include the declaration of the current symbol.
  [[nodiscard]] bool includeDeclaration();
  void set_includeDeclaration(bool val);

  void init();
};

/**
 * Captures why the code action is currently disabled.
 * 
 * @since 3.18.0
 */
struct CodeActionDisabled : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Human readable description of why the code action is currently disabled.
  // 
  // This is displayed in the code actions UI.
  [[nodiscard]] std::string_view reason();
  void set_reason(std::string_view val);

  void init();
};

/**
 * Location with only uri and does not include range.
 * 
 * @since 3.18.0
 */
struct LocationUriOnly : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  void init();
};

/**
 * Value-object describing what options formatting should use.
 */
struct FormattingOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Size of a tab in spaces.
  [[nodiscard]] std::uint32_t tabSize();
  void set_tabSize(std::uint32_t val);

  // Prefer spaces over tabs.
  [[nodiscard]] bool insertSpaces();
  void set_insertSpaces(bool val);

  // Trim trailing whitespace on a line.
  // 
  // @since 3.15.0
  [[nodiscard]] bool trimTrailingWhitespace();
  [[nodiscard]] bool has_trimTrailingWhitespace() const;
  void set_trimTrailingWhitespace(bool val);

  // Insert a newline character at the end of the file if one does not exist.
  // 
  // @since 3.15.0
  [[nodiscard]] bool insertFinalNewline();
  [[nodiscard]] bool has_insertFinalNewline() const;
  void set_insertFinalNewline(bool val);

  // Trim all newlines after the final newline at the end of the file.
  // 
  // @since 3.15.0
  [[nodiscard]] bool trimFinalNewlines();
  [[nodiscard]] bool has_trimFinalNewlines() const;
  void set_trimFinalNewlines(bool val);

  void init();
};

/**
 * Provider options for a {@link DocumentOnTypeFormattingRequest}.
 */
struct DocumentOnTypeFormattingOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A character on which formatting should be triggered, like `{`.
  [[nodiscard]] std::string_view firstTriggerCharacter();
  void set_firstTriggerCharacter(std::string_view val);

  // More trigger characters.
  [[nodiscard]] List<std::string_view> moreTriggerCharacter();
  [[nodiscard]] bool has_moreTriggerCharacter() const;
  [[nodiscard]] List<std::string_view> add_moreTriggerCharacter();

  void init();
};

/**
 * Additional data about a workspace edit.
 * 
 * @since 3.18.0
 * @proposed
 */
struct WorkspaceEditMetadata : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Signal to the editor that this edit is a refactoring.
  [[nodiscard]] bool isRefactoring();
  [[nodiscard]] bool has_isRefactoring() const;
  void set_isRefactoring(bool val);

  void init();
};

/**
 * The result returned from the apply workspace edit request.
 * 
 * @since 3.17 renamed from ApplyWorkspaceEditResponse
 */
struct ApplyWorkspaceEditResult : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Indicates whether the edit was applied or not.
  [[nodiscard]] bool applied();
  void set_applied(bool val);

  // An optional textual description for why the edit was not applied.
  // This may be used by the server for diagnostic logging or to provide
  // a suitable error for a request that triggered the edit.
  [[nodiscard]] std::string_view failureReason();
  [[nodiscard]] bool has_failureReason() const;
  void set_failureReason(std::string_view val);

  // Depending on the client's failure handling strategy `failedChange` might
  // contain the index of the change that failed. This property is only available
  // if the client signals a `failureHandlingStrategy` in its client capabilities.
  [[nodiscard]] std::uint32_t failedChange();
  [[nodiscard]] bool has_failedChange() const;
  void set_failedChange(std::uint32_t val);

  void init();
};

struct WorkDoneProgressBegin : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Mandatory title of the progress operation. Used to briefly inform about
  // the kind of operation being performed.
  // 
  // Examples: "Indexing" or "Linking dependencies".
  [[nodiscard]] std::string_view title();
  void set_title(std::string_view val);

  // Controls if a cancel button should show to allow the user to cancel the
  // long running operation. Clients that don't support cancellation are allowed
  // to ignore the setting.
  [[nodiscard]] bool cancellable();
  [[nodiscard]] bool has_cancellable() const;
  void set_cancellable(bool val);

  // Optional, more detailed associated progress message. Contains
  // complementary information to the `title`.
  // 
  // Examples: "3/25 files", "project/src/module2", "node_modules/some_dep".
  // If unset, the previous progress message (if any) is still valid.
  [[nodiscard]] std::string_view message();
  [[nodiscard]] bool has_message() const;
  void set_message(std::string_view val);

  // Optional progress percentage to display (value 100 is considered 100%).
  // If not provided infinite progress is assumed and clients are allowed
  // to ignore the `percentage` value in subsequent in report notifications.
  // 
  // The value should be steadily rising. Clients are free to ignore values
  // that are not following this rule. The value range is [0, 100].
  [[nodiscard]] std::uint32_t percentage();
  [[nodiscard]] bool has_percentage() const;
  void set_percentage(std::uint32_t val);

  void init();
};

struct WorkDoneProgressReport : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Controls enablement state of a cancel button.
  // 
  // Clients that don't support cancellation or don't support controlling the button's
  // enablement state are allowed to ignore the property.
  [[nodiscard]] bool cancellable();
  [[nodiscard]] bool has_cancellable() const;
  void set_cancellable(bool val);

  // Optional, more detailed associated progress message. Contains
  // complementary information to the `title`.
  // 
  // Examples: "3/25 files", "project/src/module2", "node_modules/some_dep".
  // If unset, the previous progress message (if any) is still valid.
  [[nodiscard]] std::string_view message();
  [[nodiscard]] bool has_message() const;
  void set_message(std::string_view val);

  // Optional progress percentage to display (value 100 is considered 100%).
  // If not provided infinite progress is assumed and clients are allowed
  // to ignore the `percentage` value in subsequent in report notifications.
  // 
  // The value should be steadily rising. Clients are free to ignore values
  // that are not following this rule. The value range is [0, 100]
  [[nodiscard]] std::uint32_t percentage();
  [[nodiscard]] bool has_percentage() const;
  void set_percentage(std::uint32_t val);

  void init();
};

struct WorkDoneProgressEnd : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Optional, a final message indicating to for example indicate the outcome
  // of the operation.
  [[nodiscard]] std::string_view message();
  [[nodiscard]] bool has_message() const;
  void set_message(std::string_view val);

  void init();
};

struct LogTraceParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] std::string_view message();
  void set_message(std::string_view val);
  [[nodiscard]] std::string_view verbose();
  [[nodiscard]] bool has_verbose() const;
  void set_verbose(std::string_view val);

  void init();
};

struct CancelParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The request id to cancel.
  struct Union0 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] CancelParams::Union0 id();

  void init();
};

/**
 * @since 3.16.0
 */
struct SemanticTokensLegend : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The token types a server uses.
  [[nodiscard]] List<std::string_view> tokenTypes();

  // The token modifiers a server uses.
  [[nodiscard]] List<std::string_view> tokenModifiers();

  void init();
};

/**
 * Semantic tokens options to support deltas for full documents
 * 
 * @since 3.18.0
 */
struct SemanticTokensFullDelta : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The server supports deltas for full documents.
  [[nodiscard]] bool delta();
  [[nodiscard]] bool has_delta() const;
  void set_delta(bool val);

  void init();
};

/**
 * Options to create a file.
 */
struct CreateFileOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Overwrite existing file. Overwrite wins over `ignoreIfExists`
  [[nodiscard]] bool overwrite();
  [[nodiscard]] bool has_overwrite() const;
  void set_overwrite(bool val);

  // Ignore if exists.
  [[nodiscard]] bool ignoreIfExists();
  [[nodiscard]] bool has_ignoreIfExists() const;
  void set_ignoreIfExists(bool val);

  void init();
};

/**
 * Rename file options
 */
struct RenameFileOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Overwrite target if existing. Overwrite wins over `ignoreIfExists`
  [[nodiscard]] bool overwrite();
  [[nodiscard]] bool has_overwrite() const;
  void set_overwrite(bool val);

  // Ignores if target exists.
  [[nodiscard]] bool ignoreIfExists();
  [[nodiscard]] bool has_ignoreIfExists() const;
  void set_ignoreIfExists(bool val);

  void init();
};

/**
 * Delete file options
 */
struct DeleteFileOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Delete the content recursively if a folder is denoted.
  [[nodiscard]] bool recursive();
  [[nodiscard]] bool has_recursive() const;
  void set_recursive(bool val);

  // Ignore the operation if the file doesn't exist.
  [[nodiscard]] bool ignoreIfNotExists();
  [[nodiscard]] bool has_ignoreIfNotExists() const;
  void set_ignoreIfNotExists(bool val);

  void init();
};

/**
 * Information about the client
 * 
 * @since 3.15.0
 * @since 3.18.0 ClientInfo type name added.
 */
struct ClientInfo : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The name of the client as defined by the client.
  [[nodiscard]] std::string_view name();
  void set_name(std::string_view val);

  // The client's version as defined by the client.
  [[nodiscard]] std::string_view version();
  [[nodiscard]] bool has_version() const;
  void set_version(std::string_view val);

  void init();
};

/**
 * Structure to capture a description for an error code.
 * 
 * @since 3.16.0
 */
struct CodeDescription : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // An URI to open with more information about the diagnostic error.
  [[nodiscard]] std::string_view href();
  void set_href(std::string_view val);

  void init();
};

/**
 * @since 3.18.0
 */
struct ServerCompletionItemOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The server has support for completion item label
  // details (see also `CompletionItemLabelDetails`) when
  // receiving a completion item in a resolve call.
  // 
  // @since 3.17.0
  [[nodiscard]] bool labelDetailsSupport();
  [[nodiscard]] bool has_labelDetailsSupport() const;
  void set_labelDetailsSupport(bool val);

  void init();
};

/**
 * Matching options for the file operation pattern.
 * 
 * @since 3.16.0
 */
struct FileOperationPatternOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The pattern should be matched ignoring casing.
  [[nodiscard]] bool ignoreCase();
  [[nodiscard]] bool has_ignoreCase() const;
  void set_ignoreCase(bool val);

  void init();
};

struct ExecutionSummary : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A strict monotonically increasing value
  // indicating the execution order of a cell
  // inside a notebook.
  [[nodiscard]] std::uint32_t executionOrder();
  void set_executionOrder(std::uint32_t val);

  // Whether the execution was successful or
  // not if known by the client.
  [[nodiscard]] bool success();
  [[nodiscard]] bool has_success() const;
  void set_success(bool val);

  void init();
};

/**
 * @since 3.18.0
 */
struct NotebookCellLanguage : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] std::string_view language();
  void set_language(std::string_view val);

  void init();
};

struct WorkspaceFoldersServerCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The server has support for workspace folders
  [[nodiscard]] bool supported();
  [[nodiscard]] bool has_supported() const;
  void set_supported(bool val);

  // Whether the server wants to receive workspace folder
  // change notifications.
  // 
  // If a string is provided the string is treated as an ID
  // under which the notification is registered on the client
  // side. The ID can be used to unregister for these events
  // using the `client/unregisterCapability` request.
  struct Union1 : detail::UnionStructWrapper<std::string_view, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] WorkspaceFoldersServerCapabilities::Union1 changeNotifications();
  [[nodiscard]] bool has_changeNotifications() const;
  [[nodiscard]] WorkspaceFoldersServerCapabilities::Union1 add_changeNotifications();

  void init();
};

struct DidChangeConfigurationClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Did change configuration notification supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  void init();
};

struct DidChangeWatchedFilesClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Did change watched files notification supports dynamic registration. Please note
  // that the current protocol doesn't support static configuration for file changes
  // from the server side.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // Whether the client has support for {@link  RelativePattern relative pattern}
  // or not.
  // 
  // @since 3.17.0
  [[nodiscard]] bool relativePatternSupport();
  [[nodiscard]] bool has_relativePatternSupport() const;
  void set_relativePatternSupport(bool val);

  void init();
};

/**
 * The client capabilities of a {@link ExecuteCommandRequest}.
 */
struct ExecuteCommandClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Execute command supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  void init();
};

/**
 * @since 3.16.0
 */
struct SemanticTokensWorkspaceClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether the client implementation supports a refresh request sent from
  // the server to the client.
  // 
  // Note that this event is global and will force the client to refresh all
  // semantic tokens currently shown. It should be used with absolute care
  // and is useful for situation where a server for example detects a project
  // wide change that requires such a calculation.
  [[nodiscard]] bool refreshSupport();
  [[nodiscard]] bool has_refreshSupport() const;
  void set_refreshSupport(bool val);

  void init();
};

/**
 * @since 3.16.0
 */
struct CodeLensWorkspaceClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether the client implementation supports a refresh request sent from the
  // server to the client.
  // 
  // Note that this event is global and will force the client to refresh all
  // code lenses currently shown. It should be used with absolute care and is
  // useful for situation where a server for example detect a project wide
  // change that requires such a calculation.
  [[nodiscard]] bool refreshSupport();
  [[nodiscard]] bool has_refreshSupport() const;
  void set_refreshSupport(bool val);

  void init();
};

/**
 * Capabilities relating to events from file operations by the user in the client.
 * 
 * These events do not come from the file system, they come from user operations
 * like renaming a file in the UI.
 * 
 * @since 3.16.0
 */
struct FileOperationClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether the client supports dynamic registration for file requests/notifications.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // The client has support for sending didCreateFiles notifications.
  [[nodiscard]] bool didCreate();
  [[nodiscard]] bool has_didCreate() const;
  void set_didCreate(bool val);

  // The client has support for sending willCreateFiles requests.
  [[nodiscard]] bool willCreate();
  [[nodiscard]] bool has_willCreate() const;
  void set_willCreate(bool val);

  // The client has support for sending didRenameFiles notifications.
  [[nodiscard]] bool didRename();
  [[nodiscard]] bool has_didRename() const;
  void set_didRename(bool val);

  // The client has support for sending willRenameFiles requests.
  [[nodiscard]] bool willRename();
  [[nodiscard]] bool has_willRename() const;
  void set_willRename(bool val);

  // The client has support for sending didDeleteFiles notifications.
  [[nodiscard]] bool didDelete();
  [[nodiscard]] bool has_didDelete() const;
  void set_didDelete(bool val);

  // The client has support for sending willDeleteFiles requests.
  [[nodiscard]] bool willDelete();
  [[nodiscard]] bool has_willDelete() const;
  void set_willDelete(bool val);

  void init();
};

/**
 * Client workspace capabilities specific to inline values.
 * 
 * @since 3.17.0
 */
struct InlineValueWorkspaceClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether the client implementation supports a refresh request sent from the
  // server to the client.
  // 
  // Note that this event is global and will force the client to refresh all
  // inline values currently shown. It should be used with absolute care and is
  // useful for situation where a server for example detects a project wide
  // change that requires such a calculation.
  [[nodiscard]] bool refreshSupport();
  [[nodiscard]] bool has_refreshSupport() const;
  void set_refreshSupport(bool val);

  void init();
};

/**
 * Client workspace capabilities specific to inlay hints.
 * 
 * @since 3.17.0
 */
struct InlayHintWorkspaceClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether the client implementation supports a refresh request sent from
  // the server to the client.
  // 
  // Note that this event is global and will force the client to refresh all
  // inlay hints currently shown. It should be used with absolute care and
  // is useful for situation where a server for example detects a project wide
  // change that requires such a calculation.
  [[nodiscard]] bool refreshSupport();
  [[nodiscard]] bool has_refreshSupport() const;
  void set_refreshSupport(bool val);

  void init();
};

/**
 * Workspace client capabilities specific to diagnostic pull requests.
 * 
 * @since 3.17.0
 */
struct DiagnosticWorkspaceClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether the client implementation supports a refresh request sent from
  // the server to the client.
  // 
  // Note that this event is global and will force the client to refresh all
  // pulled diagnostics currently shown. It should be used with absolute care and
  // is useful for situation where a server for example detects a project wide
  // change that requires such a calculation.
  [[nodiscard]] bool refreshSupport();
  [[nodiscard]] bool has_refreshSupport() const;
  void set_refreshSupport(bool val);

  void init();
};

/**
 * Client workspace capabilities specific to folding ranges
 * 
 * @since 3.18.0
 * @proposed
 */
struct FoldingRangeWorkspaceClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether the client implementation supports a refresh request sent from the
  // server to the client.
  // 
  // Note that this event is global and will force the client to refresh all
  // folding ranges currently shown. It should be used with absolute care and is
  // useful for situation where a server for example detects a project wide
  // change that requires such a calculation.
  // 
  // @since 3.18.0
  // @proposed
  [[nodiscard]] bool refreshSupport();
  [[nodiscard]] bool has_refreshSupport() const;
  void set_refreshSupport(bool val);

  void init();
};

/**
 * Client capabilities for a text document content provider.
 * 
 * @since 3.18.0
 * @proposed
 */
struct TextDocumentContentClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Text document content provider supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  void init();
};

struct TextDocumentSyncClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether text document synchronization supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // The client supports sending will save notifications.
  [[nodiscard]] bool willSave();
  [[nodiscard]] bool has_willSave() const;
  void set_willSave(bool val);

  // The client supports sending a will save request and
  // waits for a response providing text edits which will
  // be applied to the document before it is saved.
  [[nodiscard]] bool willSaveWaitUntil();
  [[nodiscard]] bool has_willSaveWaitUntil() const;
  void set_willSaveWaitUntil(bool val);

  // The client supports did save notifications.
  [[nodiscard]] bool didSave();
  [[nodiscard]] bool has_didSave() const;
  void set_didSave(bool val);

  void init();
};

struct TextDocumentFilterClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The client supports Relative Patterns.
  // 
  // @since 3.18.0
  [[nodiscard]] bool relativePatternSupport();
  [[nodiscard]] bool has_relativePatternSupport() const;
  void set_relativePatternSupport(bool val);

  void init();
};

/**
 * @since 3.14.0
 */
struct DeclarationClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether declaration supports dynamic registration. If this is set to `true`
  // the client supports the new `DeclarationRegistrationOptions` return value
  // for the corresponding server capability as well.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // The client supports additional metadata in the form of declaration links.
  [[nodiscard]] bool linkSupport();
  [[nodiscard]] bool has_linkSupport() const;
  void set_linkSupport(bool val);

  void init();
};

/**
 * Client Capabilities for a {@link DefinitionRequest}.
 */
struct DefinitionClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether definition supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // The client supports additional metadata in the form of definition links.
  // 
  // @since 3.14.0
  [[nodiscard]] bool linkSupport();
  [[nodiscard]] bool has_linkSupport() const;
  void set_linkSupport(bool val);

  void init();
};

/**
 * Since 3.6.0
 */
struct TypeDefinitionClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `TypeDefinitionRegistrationOptions` return value
  // for the corresponding server capability as well.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // The client supports additional metadata in the form of definition links.
  // 
  // Since 3.14.0
  [[nodiscard]] bool linkSupport();
  [[nodiscard]] bool has_linkSupport() const;
  void set_linkSupport(bool val);

  void init();
};

/**
 * @since 3.6.0
 */
struct ImplementationClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `ImplementationRegistrationOptions` return value
  // for the corresponding server capability as well.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // The client supports additional metadata in the form of definition links.
  // 
  // @since 3.14.0
  [[nodiscard]] bool linkSupport();
  [[nodiscard]] bool has_linkSupport() const;
  void set_linkSupport(bool val);

  void init();
};

/**
 * Client Capabilities for a {@link ReferencesRequest}.
 */
struct ReferenceClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether references supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  void init();
};

/**
 * Client Capabilities for a {@link DocumentHighlightRequest}.
 */
struct DocumentHighlightClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether document highlight supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  void init();
};

/**
 * The client capabilities of a {@link DocumentLinkRequest}.
 */
struct DocumentLinkClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether document link supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // Whether the client supports the `tooltip` property on `DocumentLink`.
  // 
  // @since 3.15.0
  [[nodiscard]] bool tooltipSupport();
  [[nodiscard]] bool has_tooltipSupport() const;
  void set_tooltipSupport(bool val);

  void init();
};

struct DocumentColorClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `DocumentColorRegistrationOptions` return value
  // for the corresponding server capability as well.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  void init();
};

/**
 * Client capabilities of a {@link DocumentFormattingRequest}.
 */
struct DocumentFormattingClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether formatting supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  void init();
};

/**
 * Client capabilities of a {@link DocumentRangeFormattingRequest}.
 */
struct DocumentRangeFormattingClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether range formatting supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // Whether the client supports formatting multiple ranges at once.
  // 
  // @since 3.18.0
  // @proposed
  [[nodiscard]] bool rangesSupport();
  [[nodiscard]] bool has_rangesSupport() const;
  void set_rangesSupport(bool val);

  void init();
};

/**
 * Client capabilities of a {@link DocumentOnTypeFormattingRequest}.
 */
struct DocumentOnTypeFormattingClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether on type formatting supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  void init();
};

struct SelectionRangeClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether implementation supports dynamic registration for selection range providers. If this is set to `true`
  // the client supports the new `SelectionRangeRegistrationOptions` return value for the corresponding server
  // capability as well.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  void init();
};

/**
 * @since 3.16.0
 */
struct CallHierarchyClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)`
  // return value for the corresponding server capability as well.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  void init();
};

/**
 * Client capabilities for the linked editing range request.
 * 
 * @since 3.16.0
 */
struct LinkedEditingRangeClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)`
  // return value for the corresponding server capability as well.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  void init();
};

/**
 * Client capabilities specific to the moniker request.
 * 
 * @since 3.16.0
 */
struct MonikerClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether moniker supports dynamic registration. If this is set to `true`
  // the client supports the new `MonikerRegistrationOptions` return value
  // for the corresponding server capability as well.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  void init();
};

/**
 * @since 3.17.0
 */
struct TypeHierarchyClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)`
  // return value for the corresponding server capability as well.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  void init();
};

/**
 * Client capabilities specific to inline values.
 * 
 * @since 3.17.0
 */
struct InlineValueClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether implementation supports dynamic registration for inline value providers.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  void init();
};

/**
 * Client capabilities specific to inline completions.
 * 
 * @since 3.18.0
 * @proposed
 */
struct InlineCompletionClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether implementation supports dynamic registration for inline completion providers.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  void init();
};

/**
 * Notebook specific client capabilities.
 * 
 * @since 3.17.0
 */
struct NotebookDocumentSyncClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether implementation supports dynamic registration. If this is
  // set to `true` the client supports the new
  // `(TextDocumentRegistrationOptions & StaticRegistrationOptions)`
  // return value for the corresponding server capability as well.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // The client supports sending execution summary data per cell.
  [[nodiscard]] bool executionSummarySupport();
  [[nodiscard]] bool has_executionSummarySupport() const;
  void set_executionSummarySupport(bool val);

  void init();
};

/**
 * Client capabilities for the showDocument request.
 * 
 * @since 3.16.0
 */
struct ShowDocumentClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The client has support for the showDocument
  // request.
  [[nodiscard]] bool support();
  void set_support(bool val);

  void init();
};

/**
 * @since 3.18.0
 */
struct StaleRequestSupportOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The client will actively cancel the request.
  [[nodiscard]] bool cancel();
  void set_cancel(bool val);

  // The list of requests for which the client
  // will retry the request if it receives a
  // response with error code `ContentModified`
  [[nodiscard]] List<std::string_view> retryOnContentModified();

  void init();
};

/**
 * Client capabilities specific to the used markdown parser.
 * 
 * @since 3.16.0
 */
struct MarkdownClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The name of the parser.
  [[nodiscard]] std::string_view parser();
  void set_parser(std::string_view val);

  // The version of the parser.
  [[nodiscard]] std::string_view version();
  [[nodiscard]] bool has_version() const;
  void set_version(std::string_view val);

  // A list of HTML tags that the client allows / supports in
  // Markdown.
  // 
  // @since 3.17.0
  [[nodiscard]] List<std::string_view> allowedTags();
  [[nodiscard]] bool has_allowedTags() const;
  [[nodiscard]] List<std::string_view> add_allowedTags();

  void init();
};

/**
 * @since 3.18.0
 */
struct ChangeAnnotationsSupportOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether the client groups edits with equal labels into tree nodes,
  // for instance all edits labelled with "Changes in Strings" would
  // be a tree node.
  [[nodiscard]] bool groupsOnLabel();
  [[nodiscard]] bool has_groupsOnLabel() const;
  void set_groupsOnLabel(bool val);

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientSymbolResolveOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The properties that a client can resolve lazily. Usually
  // `location.range`
  [[nodiscard]] List<std::string_view> properties();

  void init();
};

/**
 * The client supports the following `CompletionList` specific
 * capabilities.
 * 
 * @since 3.17.0
 */
struct CompletionListCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The client supports the following itemDefaults on
  // a completion list.
  // 
  // The value lists the supported property names of the
  // `CompletionList.itemDefaults` object. If omitted
  // no properties are supported.
  // 
  // @since 3.17.0
  [[nodiscard]] List<std::string_view> itemDefaults();
  [[nodiscard]] bool has_itemDefaults() const;
  [[nodiscard]] List<std::string_view> add_itemDefaults();

  // Specifies whether the client supports `CompletionList.applyKind` to
  // indicate how supported values from `completionList.itemDefaults`
  // and `completion` will be combined.
  // 
  // If a client supports `applyKind` it must support it for all fields
  // that it supports that are listed in `CompletionList.applyKind`. This
  // means when clients add support for new/future fields in completion
  // items the MUST also support merge for them if those fields are
  // defined in `CompletionList.applyKind`.
  // 
  // @since 3.18.0
  [[nodiscard]] bool applyKindSupport();
  [[nodiscard]] bool has_applyKindSupport() const;
  void set_applyKindSupport(bool val);

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientCodeActionResolveOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The properties that a client can resolve lazily.
  [[nodiscard]] List<std::string_view> properties();

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientCodeLensResolveOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The properties that a client can resolve lazily.
  [[nodiscard]] List<std::string_view> properties();

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientFoldingRangeOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // If set, the client signals that it supports setting collapsedText on
  // folding ranges to display custom labels instead of the default text.
  // 
  // @since 3.17.0
  [[nodiscard]] bool collapsedText();
  [[nodiscard]] bool has_collapsedText() const;
  void set_collapsedText(bool val);

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientInlayHintResolveOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The properties that a client can resolve lazily.
  [[nodiscard]] List<std::string_view> properties();

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientShowMessageActionItemOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether the client supports additional attributes which
  // are preserved and send back to the server in the
  // request's response.
  [[nodiscard]] bool additionalPropertiesSupport();
  [[nodiscard]] bool has_additionalPropertiesSupport() const;
  void set_additionalPropertiesSupport(bool val);

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientCompletionItemResolveOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The properties that a client can resolve lazily.
  [[nodiscard]] List<std::string_view> properties();

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientSignatureParameterInformationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The client supports processing label offsets instead of a
  // simple label string.
  // 
  // @since 3.14.0
  [[nodiscard]] bool labelOffsetSupport();
  [[nodiscard]] bool has_labelOffsetSupport() const;
  void set_labelOffsetSupport(bool val);

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientSemanticTokensRequestFullDelta : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The client will send the `textDocument/semanticTokens/full/delta` request if
  // the server provides a corresponding handler.
  [[nodiscard]] bool delta();
  [[nodiscard]] bool has_delta() const;
  void set_delta(bool val);

  void init();
};

/**
 * Represents a folding range. To be valid, start and end line must be bigger than zero and smaller
 * than the number of lines in the document. Clients are free to ignore invalid ranges.
 */
struct FoldingRange : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The zero-based start line of the range to fold. The folded area starts after the line's last character.
  // To be valid, the end must be zero or larger and smaller than the number of lines in the document.
  [[nodiscard]] std::uint32_t startLine();
  void set_startLine(std::uint32_t val);

  // The zero-based character offset from where the folded range starts. If not defined, defaults to the length of the start line.
  [[nodiscard]] std::uint32_t startCharacter();
  [[nodiscard]] bool has_startCharacter() const;
  void set_startCharacter(std::uint32_t val);

  // The zero-based end line of the range to fold. The folded area ends with the line's last character.
  // To be valid, the end must be zero or larger and smaller than the number of lines in the document.
  [[nodiscard]] std::uint32_t endLine();
  void set_endLine(std::uint32_t val);

  // The zero-based character offset before the folded range ends. If not defined, defaults to the length of the end line.
  [[nodiscard]] std::uint32_t endCharacter();
  [[nodiscard]] bool has_endCharacter() const;
  void set_endCharacter(std::uint32_t val);

  // Describes the kind of the folding range such as 'comment' or 'region'. The kind
  // is used to categorize folding ranges and used by commands like 'Fold all comments'.
  // See {@link FoldingRangeKind} for an enumeration of standardized kinds.
  [[nodiscard]] FoldingRangeKind kind();
  [[nodiscard]] bool has_kind() const;
  void set_kind(FoldingRangeKind val);

  // The text that the client should show when the specified range is
  // collapsed. If not defined or not supported by the client, a default
  // will be chosen by the client.
  // 
  // @since 3.17.0
  [[nodiscard]] std::string_view collapsedText();
  [[nodiscard]] bool has_collapsedText() const;
  void set_collapsedText(std::string_view val);

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientFoldingRangeKindOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The folding range kind values the client supports. When this
  // property exists the client also guarantees that it will
  // handle values outside its set gracefully and falls back
  // to a default value when unknown.
  [[nodiscard]] List<FoldingRangeKind> valueSet();
  [[nodiscard]] bool has_valueSet() const;
  [[nodiscard]] List<FoldingRangeKind> add_valueSet();

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientSymbolKindOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The symbol kind values the client supports. When this
  // property exists the client also guarantees that it will
  // handle values outside its set gracefully and falls back
  // to a default value when unknown.
  // 
  // If this property is not present the client only supports
  // the symbol kinds from `File` to `Array` as defined in
  // the initial version of the protocol.
  [[nodiscard]] List<SymbolKind> valueSet();
  [[nodiscard]] bool has_valueSet() const;
  [[nodiscard]] List<SymbolKind> add_valueSet();

  void init();
};

/**
 * A base for all symbol information.
 */
struct BaseSymbolInformation : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The name of this symbol.
  [[nodiscard]] std::string_view name();
  void set_name(std::string_view val);

  // The kind of this symbol.
  [[nodiscard]] SymbolKind kind();
  void set_kind(SymbolKind val);

  // Tags for this symbol.
  // 
  // @since 3.16.0
  [[nodiscard]] List<SymbolTag> tags();
  [[nodiscard]] bool has_tags() const;
  [[nodiscard]] List<SymbolTag> add_tags();

  // The name of the symbol containing this symbol. This information is for
  // user interface purposes (e.g. to render a qualifier in the user interface
  // if necessary). It can't be used to re-infer a hierarchy for the document
  // symbols.
  [[nodiscard]] std::string_view containerName();
  [[nodiscard]] bool has_containerName() const;
  void set_containerName(std::string_view val);

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientSymbolTagOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The tags supported by the client.
  [[nodiscard]] List<SymbolTag> valueSet();

  void init();
};

/**
 * Moniker definition to match LSIF 0.5 moniker definition.
 * 
 * @since 3.16.0
 */
struct Moniker : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The scheme of the moniker. For example tsc or .Net
  [[nodiscard]] std::string_view scheme();
  void set_scheme(std::string_view val);

  // The identifier of the moniker. The value is opaque in LSIF however
  // schema owners are allowed to define the structure if they want.
  [[nodiscard]] std::string_view identifier();
  void set_identifier(std::string_view val);

  // The scope in which the moniker is unique
  [[nodiscard]] UniquenessLevel unique();
  void set_unique(UniquenessLevel val);

  // The moniker kind if known.
  [[nodiscard]] MonikerKind kind();
  [[nodiscard]] bool has_kind() const;
  void set_kind(MonikerKind val);

  void init();
};

/**
 * The parameters of a notification message.
 */
struct ShowMessageParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The message type. See {@link MessageType}
  [[nodiscard]] MessageType type();
  void set_type(MessageType val);

  // The actual message.
  [[nodiscard]] std::string_view message();
  void set_message(std::string_view val);

  void init();
};

/**
 * The log message parameters.
 */
struct LogMessageParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The message type. See {@link MessageType}
  [[nodiscard]] MessageType type();
  void set_type(MessageType val);

  // The actual message.
  [[nodiscard]] std::string_view message();
  void set_message(std::string_view val);

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientCompletionItemOptionsKind : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The completion item kind values the client supports. When this
  // property exists the client also guarantees that it will
  // handle values outside its set gracefully and falls back
  // to a default value when unknown.
  // 
  // If this property is not present the client only supports
  // the completion items kinds from `Text` to `Reference` as defined in
  // the initial version of the protocol.
  [[nodiscard]] List<CompletionItemKind> valueSet();
  [[nodiscard]] bool has_valueSet() const;
  [[nodiscard]] List<CompletionItemKind> add_valueSet();

  void init();
};

/**
 * @since 3.18.0
 */
struct CompletionItemTagOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The tags supported by the client.
  [[nodiscard]] List<CompletionItemTag> valueSet();

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientCompletionItemInsertTextModeOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] List<InsertTextMode> valueSet();

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientCodeActionKindOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The code action kind values the client supports. When this
  // property exists the client also guarantees that it will
  // handle values outside its set gracefully and falls back
  // to a default value when unknown.
  [[nodiscard]] List<CodeActionKind> valueSet();

  void init();
};

/**
 * @since 3.18.0 - proposed
 */
struct CodeActionTagOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The tags supported by the client.
  [[nodiscard]] List<CodeActionTag> valueSet();

  void init();
};

struct SetTraceParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] TraceValue value();
  void set_value(TraceValue val);

  void init();
};

/**
 * A `MarkupContent` literal represents a string value which content is interpreted base on its
 * kind flag. Currently the protocol supports `plaintext` and `markdown` as markup kinds.
 * 
 * If the kind is `markdown` then the value can contain fenced code blocks like in GitHub issues.
 * See https://help.github.com/articles/creating-and-highlighting-code-blocks/#syntax-highlighting
 * 
 * Here is an example how such a string can be constructed using JavaScript / TypeScript:
 * ```ts
 * let markdown: MarkdownContent = {
 *  kind: MarkupKind.Markdown,
 *  value: [
 *    '# Header',
 *    'Some text',
 *    '```typescript',
 *    'someCode();',
 *    '```'
 *  ].join('\n')
 * };
 * ```
 * 
 * *Please Note* that clients might sanitize the return markdown. A client could decide to
 * remove HTML from the markdown to avoid script execution.
 */
struct MarkupContent : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The type of the Markup
  [[nodiscard]] MarkupKind kind();
  void set_kind(MarkupKind val);

  // The content itself
  [[nodiscard]] std::string_view value();
  void set_value(std::string_view val);

  void init();
};

struct HoverClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether hover supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // Client supports the following content formats for the content
  // property. The order describes the preferred format of the client.
  [[nodiscard]] List<MarkupKind> contentFormat();
  [[nodiscard]] bool has_contentFormat() const;
  [[nodiscard]] List<MarkupKind> add_contentFormat();

  void init();
};

/**
 * An item to transfer a text document from the client to the
 * server.
 */
struct TextDocumentItem : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document's uri.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  // The text document's language identifier.
  [[nodiscard]] LanguageKind languageId();
  void set_languageId(LanguageKind val);

  // The version number of this document (it will increase after each
  // change, including undo/redo).
  [[nodiscard]] std::int32_t version();
  void set_version(std::int32_t val);

  // The content of the opened text document.
  [[nodiscard]] std::string_view text();
  void set_text(std::string_view val);

  void init();
};

/**
 * An event describing a file change.
 */
struct FileEvent : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The file's uri.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  // The change type.
  [[nodiscard]] FileChangeType type();
  void set_type(FileChangeType val);

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientDiagnosticsTagOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The tags supported by the client.
  [[nodiscard]] List<DiagnosticTag> valueSet();

  void init();
};

/**
 * Contains additional information about the context in which a completion request is triggered.
 */
struct CompletionContext : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // How the completion was triggered.
  [[nodiscard]] CompletionTriggerKind triggerKind();
  void set_triggerKind(CompletionTriggerKind val);

  // The trigger character (a single character) that has trigger code complete.
  // Is undefined if `triggerKind !== CompletionTriggerKind.TriggerCharacter`
  [[nodiscard]] std::string_view triggerCharacter();
  [[nodiscard]] bool has_triggerCharacter() const;
  void set_triggerCharacter(std::string_view val);

  void init();
};

/**
 * Specifies how fields from a completion item should be combined with those
 * from `completionList.itemDefaults`.
 * 
 * If unspecified, all fields will be treated as ApplyKind.Replace.
 * 
 * If a field's value is ApplyKind.Replace, the value from a completion item (if
 * provided and not `null`) will always be used instead of the value from
 * `completionItem.itemDefaults`.
 * 
 * If a field's value is ApplyKind.Merge, the values will be merged using the rules
 * defined against each field below.
 * 
 * Servers are only allowed to return `applyKind` if the client
 * signals support for this via the `completionList.applyKindSupport`
 * capability.
 * 
 * @since 3.18.0
 */
struct CompletionItemApplyKinds : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Specifies whether commitCharacters on a completion will replace or be
  // merged with those in `completionList.itemDefaults.commitCharacters`.
  // 
  // If ApplyKind.Replace, the commit characters from the completion item will
  // always be used unless not provided, in which case those from
  // `completionList.itemDefaults.commitCharacters` will be used. An
  // empty list can be used if a completion item does not have any commit
  // characters and also should not use those from
  // `completionList.itemDefaults.commitCharacters`.
  // 
  // If ApplyKind.Merge the commitCharacters for the completion will be the
  // union of all values in both `completionList.itemDefaults.commitCharacters`
  // and the completion's own `commitCharacters`.
  // 
  // @since 3.18.0
  [[nodiscard]] ApplyKind commitCharacters();
  [[nodiscard]] bool has_commitCharacters() const;
  void set_commitCharacters(ApplyKind val);

  // Specifies whether the `data` field on a completion will replace or
  // be merged with data from `completionList.itemDefaults.data`.
  // 
  // If ApplyKind.Replace, the data from the completion item will be used if
  // provided (and not `null`), otherwise
  // `completionList.itemDefaults.data` will be used. An empty object can
  // be used if a completion item does not have any data but also should
  // not use the value from `completionList.itemDefaults.data`.
  // 
  // If ApplyKind.Merge, a shallow merge will be performed between
  // `completionList.itemDefaults.data` and the completion's own data
  // using the following rules:
  // 
  // - If a completion's `data` field is not provided (or `null`), the
  //   entire `data` field from `completionList.itemDefaults.data` will be
  //   used as-is.
  // - If a completion's `data` field is provided, each field will
  //   overwrite the field of the same name in
  //   `completionList.itemDefaults.data` but no merging of nested fields
  //   within that value will occur.
  // 
  // @since 3.18.0
  [[nodiscard]] ApplyKind data();
  [[nodiscard]] bool has_data() const;
  void set_data(ApplyKind val);

  void init();
};

struct RenameClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether rename supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // Client supports testing for validity of rename operations
  // before execution.
  // 
  // @since 3.12.0
  [[nodiscard]] bool prepareSupport();
  [[nodiscard]] bool has_prepareSupport() const;
  void set_prepareSupport(bool val);

  // Client supports the default behavior result.
  // 
  // The value indicates the default behavior used by the
  // client.
  // 
  // @since 3.16.0
  [[nodiscard]] PrepareSupportDefaultBehavior prepareSupportDefaultBehavior();
  [[nodiscard]] bool has_prepareSupportDefaultBehavior() const;
  void set_prepareSupportDefaultBehavior(PrepareSupportDefaultBehavior val);

  // Whether the client honors the change annotations in
  // text edits and resource operations returned via the
  // rename request's workspace edit by for example presenting
  // the workspace edit in the user interface and asking
  // for confirmation.
  // 
  // @since 3.16.0
  [[nodiscard]] bool honorsChangeAnnotations();
  [[nodiscard]] bool has_honorsChangeAnnotations() const;
  void set_honorsChangeAnnotations(bool val);

  void init();
};

struct WorkDoneProgressCreateParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The token to be used to report progress.
  struct Union0 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] WorkDoneProgressCreateParams::Union0 token();

  void init();
};

struct WorkDoneProgressCancelParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The token to be used to report progress.
  struct Union0 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] WorkDoneProgressCancelParams::Union0 token();

  void init();
};

struct WorkDoneProgressParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // An optional token that a server can use to report work done progress.
  struct Union0 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] WorkDoneProgressParams::Union0 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] WorkDoneProgressParams::Union0 add_workDoneToken();

  void init();
};

struct PartialResultParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union0 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] PartialResultParams::Union0 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] PartialResultParams::Union0 add_partialResultToken();

  void init();
};

/**
 * A generic resource operation.
 */
struct ResourceOperation : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The resource operation kind.
  [[nodiscard]] std::string_view kind();
  void set_kind(std::string_view val);

  // An optional annotation identifier describing the operation.
  // 
  // @since 3.16.0
  [[nodiscard]] ChangeAnnotationIdentifier annotationId();
  [[nodiscard]] bool has_annotationId() const;
  void set_annotationId(ChangeAnnotationIdentifier val);

  void init();
};

/**
 * MarkedString can be used to render human readable text. It is either a markdown string
 * or a code-block that provides a language and a code snippet. The language identifier
 * is semantically equal to the optional language identifier in fenced code blocks in GitHub
 * issues. See https://help.github.com/articles/creating-and-highlighting-code-blocks/#syntax-highlighting
 * 
 * The pair of a language and a value is an equivalent to markdown:
 * ```${language}
 * ${value}
 * ```
 * 
 * Note that markdown strings will be sanitized - that means html will be escaped.
 * @deprecated use MarkupContent instead.
 */
struct MarkedString : detail::StructWrapper {
  using StructWrapper::StructWrapper;

};

/**
 * The parameters of a change configuration notification.
 */
struct DidChangeConfigurationParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The actual changed settings
  struct Union0 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] DidChangeConfigurationParams::Union0 settings();

  void init();
};

/**
 * Represents a reference to a command. Provides a title which
 * will be used to represent a command in the UI and, optionally,
 * an array of arguments which will be passed to the command handler
 * function when invoked.
 */
struct Command : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Title of the command, like `save`.
  [[nodiscard]] std::string_view title();
  void set_title(std::string_view val);

  // An optional tooltip.
  // 
  // @since 3.18.0
  // @proposed
  [[nodiscard]] std::string_view tooltip();
  [[nodiscard]] bool has_tooltip() const;
  void set_tooltip(std::string_view val);

  // The identifier of the actual command handler.
  [[nodiscard]] std::string_view command();
  void set_command(std::string_view val);

  // Arguments that the command handler should be
  // invoked with.
  [[nodiscard]] List<LSPAny> arguments();
  [[nodiscard]] bool has_arguments() const;
  [[nodiscard]] List<LSPAny> add_arguments();

  void init();
};

struct ProgressParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The progress token provided by the client or server.
  struct Union0 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] ProgressParams::Union0 token();

  // The progress data.
  struct Union1 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] ProgressParams::Union1 value();

  void init();
};

/**
 * General parameters to register for a notification or to register a provider.
 */
struct Registration : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The id used to register the request. The id can be used to deregister
  // the request again.
  [[nodiscard]] std::string_view id();
  void set_id(std::string_view val);

  // The method / capability to register for.
  [[nodiscard]] std::string_view method();
  void set_method(std::string_view val);

  // Options necessary for the registration.
  struct Union2 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] Registration::Union2 registerOptions();
  [[nodiscard]] bool has_registerOptions() const;
  [[nodiscard]] Registration::Union2 add_registerOptions();

  void init();
};

/**
 * Client capabilities specific to regular expressions.
 * 
 * @since 3.16.0
 */
struct RegularExpressionsClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The engine's name.
  [[nodiscard]] RegularExpressionEngineKind engine();
  void set_engine(RegularExpressionEngineKind val);

  // The engine's version.
  [[nodiscard]] std::string_view version();
  [[nodiscard]] bool has_version() const;
  void set_version(std::string_view val);

  void init();
};

/**
 * The workspace folder change event.
 */
struct WorkspaceFoldersChangeEvent : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The array of added workspace folders
  [[nodiscard]] List<WorkspaceFolder> added();

  // The array of the removed workspace folders
  [[nodiscard]] List<WorkspaceFolder> removed();

  void init();
};

struct WorkspaceFoldersInitializeParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The workspace folders configured in the client when the server starts.
  // 
  // This property is only available if the client supports workspace folders.
  // It can be `null` if the client supports workspace folders but none are
  // configured.
  // 
  // @since 3.6.0
  struct Union0 : detail::UnionStructWrapper<List<WorkspaceFolder>> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_array_WorkspaceFolder() const;
    [[nodiscard]] List<WorkspaceFolder> as_array_WorkspaceFolder();

  };

  [[nodiscard]] WorkspaceFoldersInitializeParams::Union0 workspaceFolders();
  [[nodiscard]] bool has_workspaceFolders() const;
  [[nodiscard]] WorkspaceFoldersInitializeParams::Union0 add_workspaceFolders();

  void init();
};

/**
 * A relative pattern is a helper to construct glob patterns that are matched
 * relatively to a base URI. The common value for a `baseUri` is a workspace
 * folder root, but it can be another absolute URI as well.
 * 
 * @since 3.17.0
 */
struct RelativePattern : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A workspace folder or a base URI to which this pattern will be matched
  // against relatively.
  struct Union0 : detail::UnionStructWrapper<WorkspaceFolder, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_WorkspaceFolder() const;
    [[nodiscard]] WorkspaceFolder as_WorkspaceFolder();

    [[nodiscard]] bool holds_URI() const;
    [[nodiscard]] std::string_view as_URI();
    void set_URI(std::string_view val);

  };

  [[nodiscard]] RelativePattern::Union0 baseUri();

  // The actual glob pattern;
  [[nodiscard]] Pattern pattern();
  void set_pattern(Pattern val);

  void init();
};

/**
 * The parameters of a configuration request.
 */
struct ConfigurationParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] List<ConfigurationItem> items();

  void init();
};

/**
 * The parameters sent in a close text document notification
 */
struct DidCloseTextDocumentParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document that was closed.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  void init();
};

/**
 * The parameters sent in a save text document notification
 */
struct DidSaveTextDocumentParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document that was saved.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // Optional the content when saved. Depends on the includeText value
  // when the save notification was requested.
  [[nodiscard]] std::string_view text();
  [[nodiscard]] bool has_text() const;
  void set_text(std::string_view val);

  void init();
};

/**
 * The parameters sent in a will save text document notification.
 */
struct WillSaveTextDocumentParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document that will be saved.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The 'TextDocumentSaveReason'.
  [[nodiscard]] TextDocumentSaveReason reason();
  void set_reason(TextDocumentSaveReason val);

  void init();
};

/**
 * A text document identifier to denote a specific version of a text document.
 */
struct VersionedTextDocumentIdentifier : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The version number of this document.
  [[nodiscard]] std::int32_t version();
  void set_version(std::int32_t val);

  // The text document's uri.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  void init();
};

/**
 * A text document identifier to optionally denote a specific version of a text document.
 */
struct OptionalVersionedTextDocumentIdentifier : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The version number of this document. If a versioned text document identifier
  // is sent from the server to the client and the file is not open in the editor
  // (the server has not received an open notification before) the server can send
  // `null` to indicate that the version is unknown and the content on disk is the
  // truth (as specified with document content ownership).
  struct Union0 : detail::UnionStructWrapper<std::int32_t> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

  };

  [[nodiscard]] OptionalVersionedTextDocumentIdentifier::Union0 version();

  // The text document's uri.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  void init();
};

struct ImplementationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

struct TypeDefinitionOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

struct DocumentColorOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

struct FoldingRangeOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

struct DeclarationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

struct SelectionRangeOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Call hierarchy options used during static registration.
 * 
 * @since 3.16.0
 */
struct CallHierarchyOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

struct LinkedEditingRangeOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

struct MonikerOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Type hierarchy options used during static registration.
 * 
 * @since 3.17.0
 */
struct TypeHierarchyOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Inline value options used during static registration.
 * 
 * @since 3.17.0
 */
struct InlineValueOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Inlay hint options used during static registration.
 * 
 * @since 3.17.0
 */
struct InlayHintOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The server provides support to resolve additional
  // information for an inlay hint item.
  [[nodiscard]] bool resolveProvider();
  [[nodiscard]] bool has_resolveProvider() const;
  void set_resolveProvider(bool val);
  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Diagnostic options.
 * 
 * @since 3.17.0
 */
struct DiagnosticOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // An optional identifier under which the diagnostics are
  // managed by the client.
  [[nodiscard]] std::string_view identifier();
  [[nodiscard]] bool has_identifier() const;
  void set_identifier(std::string_view val);

  // Whether the language has inter file dependencies meaning that
  // editing code in one file can result in a different diagnostic
  // set in another file. Inter file dependencies are common for
  // most programming languages and typically uncommon for linters.
  [[nodiscard]] bool interFileDependencies();
  void set_interFileDependencies(bool val);

  // The server provides support for workspace diagnostics as well.
  [[nodiscard]] bool workspaceDiagnostics();
  void set_workspaceDiagnostics(bool val);
  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Inline completion options used during static registration.
 * 
 * @since 3.18.0
 * @proposed
 */
struct InlineCompletionOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Hover options.
 */
struct HoverOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Server Capabilities for a {@link SignatureHelpRequest}.
 */
struct SignatureHelpOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // List of characters that trigger signature help automatically.
  [[nodiscard]] List<std::string_view> triggerCharacters();
  [[nodiscard]] bool has_triggerCharacters() const;
  [[nodiscard]] List<std::string_view> add_triggerCharacters();

  // List of characters that re-trigger signature help.
  // 
  // These trigger characters are only active when signature help is already showing. All trigger characters
  // are also counted as re-trigger characters.
  // 
  // @since 3.15.0
  [[nodiscard]] List<std::string_view> retriggerCharacters();
  [[nodiscard]] bool has_retriggerCharacters() const;
  [[nodiscard]] List<std::string_view> add_retriggerCharacters();
  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Server Capabilities for a {@link DefinitionRequest}.
 */
struct DefinitionOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Reference options.
 */
struct ReferenceOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Provider options for a {@link DocumentHighlightRequest}.
 */
struct DocumentHighlightOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Provider options for a {@link DocumentSymbolRequest}.
 */
struct DocumentSymbolOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A human-readable string that is shown when multiple outlines trees
  // are shown for the same document.
  // 
  // @since 3.16.0
  [[nodiscard]] std::string_view label();
  [[nodiscard]] bool has_label() const;
  void set_label(std::string_view val);
  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Server capabilities for a {@link WorkspaceSymbolRequest}.
 */
struct WorkspaceSymbolOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The server provides support to resolve additional
  // information for a workspace symbol.
  // 
  // @since 3.17.0
  [[nodiscard]] bool resolveProvider();
  [[nodiscard]] bool has_resolveProvider() const;
  void set_resolveProvider(bool val);
  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Code Lens provider options of a {@link CodeLensRequest}.
 */
struct CodeLensOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Code lens has a resolve provider as well.
  [[nodiscard]] bool resolveProvider();
  [[nodiscard]] bool has_resolveProvider() const;
  void set_resolveProvider(bool val);
  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Provider options for a {@link DocumentLinkRequest}.
 */
struct DocumentLinkOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Document links have a resolve provider as well.
  [[nodiscard]] bool resolveProvider();
  [[nodiscard]] bool has_resolveProvider() const;
  void set_resolveProvider(bool val);
  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Provider options for a {@link DocumentFormattingRequest}.
 */
struct DocumentFormattingOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Provider options for a {@link DocumentRangeFormattingRequest}.
 */
struct DocumentRangeFormattingOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether the server supports formatting multiple ranges at once.
  // 
  // @since 3.18.0
  // @proposed
  [[nodiscard]] bool rangesSupport();
  [[nodiscard]] bool has_rangesSupport() const;
  void set_rangesSupport(bool val);
  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Provider options for a {@link RenameRequest}.
 */
struct RenameOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Renames should be checked and tested before being executed.
  // 
  // @since version 3.12.0
  [[nodiscard]] bool prepareProvider();
  [[nodiscard]] bool has_prepareProvider() const;
  void set_prepareProvider(bool val);
  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * The server capabilities of a {@link ExecuteCommandRequest}.
 */
struct ExecuteCommandOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The commands to be executed on the server
  [[nodiscard]] List<std::string_view> commands();
  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * A parameter literal used in requests to pass a text document and a position inside that
 * document.
 */
struct TextDocumentPositionParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  void init();
};

/**
 * A range in a text document expressed as (zero-based) start and end positions.
 * 
 * If you want to specify a range that contains a line including the line ending
 * character(s) then use an end position denoting the start of the next line.
 * For example:
 * ```ts
 * {
 *     start: { line: 5, character: 23 }
 *     end : { line 6, character : 0 }
 * }
 * ```
 */
struct Range : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The range's start position.
  [[nodiscard]] Position start();

  // The range's end position.
  [[nodiscard]] Position end();

  void init();
};

/**
 * @since 3.16.0
 */
struct SemanticTokensDelta : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] std::string_view resultId();
  [[nodiscard]] bool has_resultId() const;
  void set_resultId(std::string_view val);

  // The semantic token edits to transform a previous result into a new result.
  [[nodiscard]] List<SemanticTokensEdit> edits();

  void init();
};

/**
 * @since 3.16.0
 */
struct SemanticTokensDeltaPartialResult : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] List<SemanticTokensEdit> edits();

  void init();
};

/**
 * The parameters sent in notifications/requests for user-initiated creation of
 * files.
 * 
 * @since 3.16.0
 */
struct CreateFilesParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // An array of all files/folders created in this operation.
  [[nodiscard]] List<FileCreate> files();

  void init();
};

/**
 * The parameters sent in notifications/requests for user-initiated renames of
 * files.
 * 
 * @since 3.16.0
 */
struct RenameFilesParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // An array of all files/folders renamed in this operation. When a folder is renamed, only
  // the folder will be included, and not its children.
  [[nodiscard]] List<FileRename> files();

  void init();
};

/**
 * The parameters sent in notifications/requests for user-initiated deletes of
 * files.
 * 
 * @since 3.16.0
 */
struct DeleteFilesParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // An array of all files/folders deleted in this operation.
  [[nodiscard]] List<FileDelete> files();

  void init();
};

/**
 * An unchanged document diagnostic report for a workspace diagnostic result.
 * 
 * @since 3.17.0
 */
struct WorkspaceUnchangedDocumentDiagnosticReport : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The URI for which diagnostic information is reported.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  // The version number for which the diagnostics are reported.
  // If the document is not marked as open `null` can be provided.
  struct Union1 : detail::UnionStructWrapper<std::int32_t> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

  };

  [[nodiscard]] WorkspaceUnchangedDocumentDiagnosticReport::Union1 version();

  // A result id which will be sent on the next
  // diagnostic request for the same document.
  [[nodiscard]] std::string_view resultId();
  void set_resultId(std::string_view val);

  void init();
};

/**
 * The params sent in a save notebook document notification.
 * 
 * @since 3.17.0
 */
struct DidSaveNotebookDocumentParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The notebook document that got saved.
  [[nodiscard]] NotebookDocumentIdentifier notebookDocument();

  void init();
};

/**
 * The params sent in a close notebook document notification.
 * 
 * @since 3.17.0
 */
struct DidCloseNotebookDocumentParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The notebook document that got closed.
  [[nodiscard]] NotebookDocumentIdentifier notebookDocument();

  // The text documents that represent the content
  // of a notebook cell that got closed.
  [[nodiscard]] List<TextDocumentIdentifier> cellTextDocuments();

  void init();
};

/**
 * Text document content provider registration options.
 * 
 * @since 3.18.0
 * @proposed
 */
struct TextDocumentContentRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The schemes for which the server provides content.
  [[nodiscard]] List<std::string_view> schemes();

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

struct UnregistrationParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] List<Unregistration> unregisterations();

  void init();
};

struct ShowMessageRequestParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The message type. See {@link MessageType}
  [[nodiscard]] MessageType type();
  void set_type(MessageType val);

  // The actual message.
  [[nodiscard]] std::string_view message();
  void set_message(std::string_view val);

  // The message action items to present.
  [[nodiscard]] List<MessageActionItem> actions();
  [[nodiscard]] bool has_actions() const;
  [[nodiscard]] List<MessageActionItem> add_actions();

  void init();
};

struct TextDocumentSyncOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Open and close notifications are sent to the server. If omitted open close notification should not
  // be sent.
  [[nodiscard]] bool openClose();
  [[nodiscard]] bool has_openClose() const;
  void set_openClose(bool val);

  // Change notifications are sent to the server. See TextDocumentSyncKind.None, TextDocumentSyncKind.Full
  // and TextDocumentSyncKind.Incremental. If omitted it defaults to TextDocumentSyncKind.None.
  [[nodiscard]] TextDocumentSyncKind change();
  [[nodiscard]] bool has_change() const;
  void set_change(TextDocumentSyncKind val);

  // If present will save notifications are sent to the server. If omitted the notification should not be
  // sent.
  [[nodiscard]] bool willSave();
  [[nodiscard]] bool has_willSave() const;
  void set_willSave(bool val);

  // If present will save wait until requests are sent to the server. If omitted the request should not be
  // sent.
  [[nodiscard]] bool willSaveWaitUntil();
  [[nodiscard]] bool has_willSaveWaitUntil() const;
  void set_willSaveWaitUntil(bool val);

  // If present save notifications are sent to the server. If omitted the notification should not be
  // sent.
  struct Union4 : detail::UnionStructWrapper<bool, SaveOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_SaveOptions() const;
    [[nodiscard]] SaveOptions as_SaveOptions();

  };

  [[nodiscard]] TextDocumentSyncOptions::Union4 save();
  [[nodiscard]] bool has_save() const;
  [[nodiscard]] TextDocumentSyncOptions::Union4 add_save();

  void init();
};

/**
 * The parameters of a {@link DocumentOnTypeFormattingRequest}.
 */
struct DocumentOnTypeFormattingParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document to format.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position around which the on type formatting should happen.
  // This is not necessarily the exact position where the character denoted
  // by the property `ch` got typed.
  [[nodiscard]] Position position();

  // The character that has been typed that triggered the formatting
  // on type request. That is not necessarily the last character that
  // got inserted into the document since the client could auto insert
  // characters as well (e.g. like automatic brace completion).
  [[nodiscard]] std::string_view ch();
  void set_ch(std::string_view val);

  // The formatting options.
  [[nodiscard]] FormattingOptions options();

  void init();
};

/**
 * @since 3.16.0
 */
struct SemanticTokensOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The legend used by the server
  [[nodiscard]] SemanticTokensLegend legend();

  // Server supports providing semantic tokens for a specific range
  // of a document.
  struct Union1 : detail::UnionStructWrapper<bool, LiteralStub> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] SemanticTokensOptions::Union1 range();
  [[nodiscard]] bool has_range() const;
  [[nodiscard]] SemanticTokensOptions::Union1 add_range();

  // Server supports providing semantic tokens for a full document.
  struct Union2 : detail::UnionStructWrapper<bool, SemanticTokensFullDelta> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_SemanticTokensFullDelta() const;
    [[nodiscard]] SemanticTokensFullDelta as_SemanticTokensFullDelta();

  };

  [[nodiscard]] SemanticTokensOptions::Union2 full();
  [[nodiscard]] bool has_full() const;
  [[nodiscard]] SemanticTokensOptions::Union2 add_full();
  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * Completion options.
 */
struct CompletionOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Most tools trigger completion request automatically without explicitly requesting
  // it using a keyboard shortcut (e.g. Ctrl+Space). Typically they do so when the user
  // starts to type an identifier. For example if the user types `c` in a JavaScript file
  // code complete will automatically pop up present `console` besides others as a
  // completion item. Characters that make up identifiers don't need to be listed here.
  // 
  // If code complete should automatically be trigger on characters not being valid inside
  // an identifier (for example `.` in JavaScript) list them in `triggerCharacters`.
  [[nodiscard]] List<std::string_view> triggerCharacters();
  [[nodiscard]] bool has_triggerCharacters() const;
  [[nodiscard]] List<std::string_view> add_triggerCharacters();

  // The list of all possible characters that commit a completion. This field can be used
  // if clients don't support individual commit characters per completion item. See
  // `ClientCapabilities.textDocument.completion.completionItem.commitCharactersSupport`
  // 
  // If a server provides both `allCommitCharacters` and commit characters on an individual
  // completion item the ones on the completion item win.
  // 
  // @since 3.2.0
  [[nodiscard]] List<std::string_view> allCommitCharacters();
  [[nodiscard]] bool has_allCommitCharacters() const;
  [[nodiscard]] List<std::string_view> add_allCommitCharacters();

  // The server provides support to resolve additional
  // information for a completion item.
  [[nodiscard]] bool resolveProvider();
  [[nodiscard]] bool has_resolveProvider() const;
  void set_resolveProvider(bool val);

  // The server supports the following `CompletionItem` specific
  // capabilities.
  // 
  // @since 3.17.0
  [[nodiscard]] ServerCompletionItemOptions completionItem();
  [[nodiscard]] bool has_completionItem() const;
  [[nodiscard]] ServerCompletionItemOptions add_completionItem();
  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

/**
 * A pattern to describe in which file operation requests or notifications
 * the server is interested in receiving.
 * 
 * @since 3.16.0
 */
struct FileOperationPattern : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The glob pattern to match. Glob patterns can have the following syntax:
  // - `*` to match one or more characters in a path segment
  // - `?` to match on one character in a path segment
  // - `**` to match any number of path segments, including none
  // - `{}` to group sub patterns into an OR expression. (e.g. `**​/\*.{ts,js}` matches all TypeScript and JavaScript files)
  // - `[]` to declare a range of characters to match in a path segment (e.g., `example.[0-9]` to match on `example.0`, `example.1`, …)
  // - `[!...]` to negate a range of characters to match in a path segment (e.g., `example.[!0-9]` to match on `example.a`, `example.b`, but not `example.0`)
  [[nodiscard]] std::string_view glob();
  void set_glob(std::string_view val);

  // Whether to match files or folders with this pattern.
  // 
  // Matches both if undefined.
  [[nodiscard]] FileOperationPatternKind matches();
  [[nodiscard]] bool has_matches() const;
  void set_matches(FileOperationPatternKind val);

  // Additional options used during matching.
  [[nodiscard]] FileOperationPatternOptions options();
  [[nodiscard]] bool has_options() const;
  [[nodiscard]] FileOperationPatternOptions add_options();

  void init();
};

/**
 * Capabilities specific to the notebook document support.
 * 
 * @since 3.17.0
 */
struct NotebookDocumentClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Capabilities specific to notebook document synchronization
  // 
  // @since 3.17.0
  [[nodiscard]] NotebookDocumentSyncClientCapabilities synchronization();

  void init();
};

struct WorkspaceEditClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The client supports versioned document changes in `WorkspaceEdit`s
  [[nodiscard]] bool documentChanges();
  [[nodiscard]] bool has_documentChanges() const;
  void set_documentChanges(bool val);

  // The resource operations the client supports. Clients should at least
  // support 'create', 'rename' and 'delete' files and folders.
  // 
  // @since 3.13.0
  [[nodiscard]] List<ResourceOperationKind> resourceOperations();
  [[nodiscard]] bool has_resourceOperations() const;
  [[nodiscard]] List<ResourceOperationKind> add_resourceOperations();

  // The failure handling strategy of a client if applying the workspace edit
  // fails.
  // 
  // @since 3.13.0
  [[nodiscard]] FailureHandlingKind failureHandling();
  [[nodiscard]] bool has_failureHandling() const;
  void set_failureHandling(FailureHandlingKind val);

  // Whether the client normalizes line endings to the client specific
  // setting.
  // If set to `true` the client will normalize line ending characters
  // in a workspace edit to the client-specified new line
  // character.
  // 
  // @since 3.16.0
  [[nodiscard]] bool normalizesLineEndings();
  [[nodiscard]] bool has_normalizesLineEndings() const;
  void set_normalizesLineEndings(bool val);

  // Whether the client in general supports change annotations on text edits,
  // create file, rename file and delete file changes.
  // 
  // @since 3.16.0
  [[nodiscard]] ChangeAnnotationsSupportOptions changeAnnotationSupport();
  [[nodiscard]] bool has_changeAnnotationSupport() const;
  [[nodiscard]] ChangeAnnotationsSupportOptions add_changeAnnotationSupport();

  // Whether the client supports `WorkspaceEditMetadata` in `WorkspaceEdit`s.
  // 
  // @since 3.18.0
  // @proposed
  [[nodiscard]] bool metadataSupport();
  [[nodiscard]] bool has_metadataSupport() const;
  void set_metadataSupport(bool val);

  // Whether the client supports snippets as text edits.
  // 
  // @since 3.18.0
  // @proposed
  [[nodiscard]] bool snippetEditSupport();
  [[nodiscard]] bool has_snippetEditSupport() const;
  void set_snippetEditSupport(bool val);

  void init();
};

/**
 * The client capabilities  of a {@link CodeLensRequest}.
 */
struct CodeLensClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether code lens supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // Whether the client supports resolving additional code lens
  // properties via a separate `codeLens/resolve` request.
  // 
  // @since 3.18.0
  [[nodiscard]] ClientCodeLensResolveOptions resolveSupport();
  [[nodiscard]] bool has_resolveSupport() const;
  [[nodiscard]] ClientCodeLensResolveOptions add_resolveSupport();

  void init();
};

/**
 * Inlay hint client capabilities.
 * 
 * @since 3.17.0
 */
struct InlayHintClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether inlay hints support dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // Indicates which properties a client can resolve lazily on an inlay
  // hint.
  [[nodiscard]] ClientInlayHintResolveOptions resolveSupport();
  [[nodiscard]] bool has_resolveSupport() const;
  [[nodiscard]] ClientInlayHintResolveOptions add_resolveSupport();

  void init();
};

/**
 * Show message request client capabilities
 */
struct ShowMessageRequestClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Capabilities specific to the `MessageActionItem` type.
  [[nodiscard]] ClientShowMessageActionItemOptions messageActionItem();
  [[nodiscard]] bool has_messageActionItem() const;
  [[nodiscard]] ClientShowMessageActionItemOptions add_messageActionItem();

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientSignatureInformationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Client supports the following content formats for the documentation
  // property. The order describes the preferred format of the client.
  [[nodiscard]] List<MarkupKind> documentationFormat();
  [[nodiscard]] bool has_documentationFormat() const;
  [[nodiscard]] List<MarkupKind> add_documentationFormat();

  // Client capabilities specific to parameter information.
  [[nodiscard]] ClientSignatureParameterInformationOptions parameterInformation();
  [[nodiscard]] bool has_parameterInformation() const;
  [[nodiscard]] ClientSignatureParameterInformationOptions add_parameterInformation();

  // The client supports the `activeParameter` property on `SignatureInformation`
  // literal.
  // 
  // @since 3.16.0
  [[nodiscard]] bool activeParameterSupport();
  [[nodiscard]] bool has_activeParameterSupport() const;
  void set_activeParameterSupport(bool val);

  // The client supports the `activeParameter` property on
  // `SignatureHelp`/`SignatureInformation` being set to `null` to
  // indicate that no parameter should be active.
  // 
  // @since 3.18.0
  // @proposed
  [[nodiscard]] bool noActiveParameterSupport();
  [[nodiscard]] bool has_noActiveParameterSupport() const;
  void set_noActiveParameterSupport(bool val);

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientSemanticTokensRequestOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The client will send the `textDocument/semanticTokens/range` request if
  // the server provides a corresponding handler.
  struct Union0 : detail::UnionStructWrapper<bool, LiteralStub> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] ClientSemanticTokensRequestOptions::Union0 range();
  [[nodiscard]] bool has_range() const;
  [[nodiscard]] ClientSemanticTokensRequestOptions::Union0 add_range();

  // The client will send the `textDocument/semanticTokens/full` request if
  // the server provides a corresponding handler.
  struct Union1 : detail::UnionStructWrapper<bool, ClientSemanticTokensRequestFullDelta> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_ClientSemanticTokensRequestFullDelta() const;
    [[nodiscard]] ClientSemanticTokensRequestFullDelta as_ClientSemanticTokensRequestFullDelta();

  };

  [[nodiscard]] ClientSemanticTokensRequestOptions::Union1 full();
  [[nodiscard]] bool has_full() const;
  [[nodiscard]] ClientSemanticTokensRequestOptions::Union1 add_full();

  void init();
};

struct FoldingRangeClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether implementation supports dynamic registration for folding range
  // providers. If this is set to `true` the client supports the new
  // `FoldingRangeRegistrationOptions` return value for the corresponding
  // server capability as well.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // The maximum number of folding ranges that the client prefers to receive
  // per document. The value serves as a hint, servers are free to follow the
  // limit.
  [[nodiscard]] std::uint32_t rangeLimit();
  [[nodiscard]] bool has_rangeLimit() const;
  void set_rangeLimit(std::uint32_t val);

  // If set, the client signals that it only supports folding complete lines.
  // If set, client will ignore specified `startCharacter` and `endCharacter`
  // properties in a FoldingRange.
  [[nodiscard]] bool lineFoldingOnly();
  [[nodiscard]] bool has_lineFoldingOnly() const;
  void set_lineFoldingOnly(bool val);

  // Specific options for the folding range kind.
  // 
  // @since 3.17.0
  [[nodiscard]] ClientFoldingRangeKindOptions foldingRangeKind();
  [[nodiscard]] bool has_foldingRangeKind() const;
  [[nodiscard]] ClientFoldingRangeKindOptions add_foldingRangeKind();

  // Specific options for the folding range.
  // 
  // @since 3.17.0
  [[nodiscard]] ClientFoldingRangeOptions foldingRange();
  [[nodiscard]] bool has_foldingRange() const;
  [[nodiscard]] ClientFoldingRangeOptions add_foldingRange();

  void init();
};

/**
 * Client capabilities for a {@link WorkspaceSymbolRequest}.
 */
struct WorkspaceSymbolClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Symbol request supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // Specific capabilities for the `SymbolKind` in the `workspace/symbol` request.
  [[nodiscard]] ClientSymbolKindOptions symbolKind();
  [[nodiscard]] bool has_symbolKind() const;
  [[nodiscard]] ClientSymbolKindOptions add_symbolKind();

  // The client supports tags on `SymbolInformation`.
  // Clients supporting tags have to handle unknown tags gracefully.
  // 
  // @since 3.16.0
  [[nodiscard]] ClientSymbolTagOptions tagSupport();
  [[nodiscard]] bool has_tagSupport() const;
  [[nodiscard]] ClientSymbolTagOptions add_tagSupport();

  // The client support partial workspace symbols. The client will send the
  // request `workspaceSymbol/resolve` to the server to resolve additional
  // properties.
  // 
  // @since 3.17.0
  [[nodiscard]] ClientSymbolResolveOptions resolveSupport();
  [[nodiscard]] bool has_resolveSupport() const;
  [[nodiscard]] ClientSymbolResolveOptions add_resolveSupport();

  void init();
};

/**
 * Client Capabilities for a {@link DocumentSymbolRequest}.
 */
struct DocumentSymbolClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether document symbol supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // Specific capabilities for the `SymbolKind` in the
  // `textDocument/documentSymbol` request.
  [[nodiscard]] ClientSymbolKindOptions symbolKind();
  [[nodiscard]] bool has_symbolKind() const;
  [[nodiscard]] ClientSymbolKindOptions add_symbolKind();

  // The client supports hierarchical document symbols.
  [[nodiscard]] bool hierarchicalDocumentSymbolSupport();
  [[nodiscard]] bool has_hierarchicalDocumentSymbolSupport() const;
  void set_hierarchicalDocumentSymbolSupport(bool val);

  // The client supports tags on `SymbolInformation`. Tags are supported on
  // `DocumentSymbol` if `hierarchicalDocumentSymbolSupport` is set to true.
  // Clients supporting tags have to handle unknown tags gracefully.
  // 
  // @since 3.16.0
  [[nodiscard]] ClientSymbolTagOptions tagSupport();
  [[nodiscard]] bool has_tagSupport() const;
  [[nodiscard]] ClientSymbolTagOptions add_tagSupport();

  // The client supports an additional label presented in the UI when
  // registering a document symbol provider.
  // 
  // @since 3.16.0
  [[nodiscard]] bool labelSupport();
  [[nodiscard]] bool has_labelSupport() const;
  void set_labelSupport(bool val);

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientCompletionItemOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Client supports snippets as insert text.
  // 
  // A snippet can define tab stops and placeholders with `$1`, `$2`
  // and `${3:foo}`. `$0` defines the final tab stop, it defaults to
  // the end of the snippet. Placeholders with equal identifiers are linked,
  // that is typing in one will update others too.
  [[nodiscard]] bool snippetSupport();
  [[nodiscard]] bool has_snippetSupport() const;
  void set_snippetSupport(bool val);

  // Client supports commit characters on a completion item.
  [[nodiscard]] bool commitCharactersSupport();
  [[nodiscard]] bool has_commitCharactersSupport() const;
  void set_commitCharactersSupport(bool val);

  // Client supports the following content formats for the documentation
  // property. The order describes the preferred format of the client.
  [[nodiscard]] List<MarkupKind> documentationFormat();
  [[nodiscard]] bool has_documentationFormat() const;
  [[nodiscard]] List<MarkupKind> add_documentationFormat();

  // Client supports the deprecated property on a completion item.
  [[nodiscard]] bool deprecatedSupport();
  [[nodiscard]] bool has_deprecatedSupport() const;
  void set_deprecatedSupport(bool val);

  // Client supports the preselect property on a completion item.
  [[nodiscard]] bool preselectSupport();
  [[nodiscard]] bool has_preselectSupport() const;
  void set_preselectSupport(bool val);

  // Client supports the tag property on a completion item. Clients supporting
  // tags have to handle unknown tags gracefully. Clients especially need to
  // preserve unknown tags when sending a completion item back to the server in
  // a resolve call.
  // 
  // @since 3.15.0
  [[nodiscard]] CompletionItemTagOptions tagSupport();
  [[nodiscard]] bool has_tagSupport() const;
  [[nodiscard]] CompletionItemTagOptions add_tagSupport();

  // Client support insert replace edit to control different behavior if a
  // completion item is inserted in the text or should replace text.
  // 
  // @since 3.16.0
  [[nodiscard]] bool insertReplaceSupport();
  [[nodiscard]] bool has_insertReplaceSupport() const;
  void set_insertReplaceSupport(bool val);

  // Indicates which properties a client can resolve lazily on a completion
  // item. Before version 3.16.0 only the predefined properties `documentation`
  // and `details` could be resolved lazily.
  // 
  // @since 3.16.0
  [[nodiscard]] ClientCompletionItemResolveOptions resolveSupport();
  [[nodiscard]] bool has_resolveSupport() const;
  [[nodiscard]] ClientCompletionItemResolveOptions add_resolveSupport();

  // The client supports the `insertTextMode` property on
  // a completion item to override the whitespace handling mode
  // as defined by the client (see `insertTextMode`).
  // 
  // @since 3.16.0
  [[nodiscard]] ClientCompletionItemInsertTextModeOptions insertTextModeSupport();
  [[nodiscard]] bool has_insertTextModeSupport() const;
  [[nodiscard]] ClientCompletionItemInsertTextModeOptions add_insertTextModeSupport();

  // The client has support for completion item label
  // details (see also `CompletionItemLabelDetails`).
  // 
  // @since 3.17.0
  [[nodiscard]] bool labelDetailsSupport();
  [[nodiscard]] bool has_labelDetailsSupport() const;
  void set_labelDetailsSupport(bool val);

  void init();
};

/**
 * @since 3.18.0
 */
struct ClientCodeActionLiteralOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The code action kind is support with the following value
  // set.
  [[nodiscard]] ClientCodeActionKindOptions codeActionKind();

  void init();
};

/**
 * Represents a parameter of a callable-signature. A parameter can
 * have a label and a doc-comment.
 */
struct ParameterInformation : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The label of this parameter information.
  // 
  // Either a string or an inclusive start and exclusive end offsets within its containing
  // signature label. (see SignatureInformation.label). The offsets are based on a UTF-16
  // string representation as `Position` and `Range` does.
  // 
  // To avoid ambiguities a server should use the [start, end] offset value instead of using
  // a substring. Whether a client support this is controlled via `labelOffsetSupport` client
  // capability.
  // 
  // *Note*: a label of type string should be a substring of its containing signature label.
  // Its intended use case is to highlight the parameter label part in the `SignatureInformation.label`.
  struct Union0 : detail::UnionStructWrapper<std::string_view, std::tuple<std::uint32_t, std::uint32_t>> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] ParameterInformation::Union0 label();

  // The human-readable doc-comment of this parameter. Will be shown
  // in the UI but can be omitted.
  struct Union1 : detail::UnionStructWrapper<std::string_view, MarkupContent> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_MarkupContent() const;
    [[nodiscard]] MarkupContent as_MarkupContent();

  };

  [[nodiscard]] ParameterInformation::Union1 documentation();
  [[nodiscard]] bool has_documentation() const;
  [[nodiscard]] ParameterInformation::Union1 add_documentation();

  void init();
};

/**
 * The parameters sent in an open text document notification
 */
struct DidOpenTextDocumentParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document that was opened.
  [[nodiscard]] TextDocumentItem textDocument();

  void init();
};

/**
 * The watched files change notification's parameters.
 */
struct DidChangeWatchedFilesParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The actual file events.
  [[nodiscard]] List<FileEvent> changes();

  void init();
};

/**
 * General diagnostics capabilities for pull and push model.
 */
struct DiagnosticsCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether the clients accepts diagnostics with related information.
  [[nodiscard]] bool relatedInformation();
  [[nodiscard]] bool has_relatedInformation() const;
  void set_relatedInformation(bool val);

  // Client supports the tag property to provide meta data about a diagnostic.
  // Clients supporting tags have to handle unknown tags gracefully.
  // 
  // @since 3.15.0
  [[nodiscard]] ClientDiagnosticsTagOptions tagSupport();
  [[nodiscard]] bool has_tagSupport() const;
  [[nodiscard]] ClientDiagnosticsTagOptions add_tagSupport();

  // Client supports a codeDescription property
  // 
  // @since 3.16.0
  [[nodiscard]] bool codeDescriptionSupport();
  [[nodiscard]] bool has_codeDescriptionSupport() const;
  void set_codeDescriptionSupport(bool val);

  // Whether code action supports the `data` property which is
  // preserved between a `textDocument/publishDiagnostics` and
  // `textDocument/codeAction` request.
  // 
  // @since 3.16.0
  [[nodiscard]] bool dataSupport();
  [[nodiscard]] bool has_dataSupport() const;
  void set_dataSupport(bool val);

  void init();
};

/**
 * The parameters of a {@link DocumentFormattingRequest}.
 */
struct DocumentFormattingParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document to format.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The format options.
  [[nodiscard]] FormattingOptions options();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DocumentFormattingParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] DocumentFormattingParams::Union2 add_workDoneToken();

  void init();
};

/**
 * The parameters of a {@link RenameRequest}.
 */
struct RenameParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document to rename.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position at which this request was sent.
  [[nodiscard]] Position position();

  // The new name of the symbol. If the given name is not valid the
  // request must return a {@link ResponseError} with an
  // appropriate message set.
  [[nodiscard]] std::string_view newName();
  void set_newName(std::string_view val);

  // An optional token that a server can use to report work done progress.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] RenameParams::Union3 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] RenameParams::Union3 add_workDoneToken();

  void init();
};

/**
 * The parameters of a {@link ExecuteCommandRequest}.
 */
struct ExecuteCommandParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The identifier of the actual command handler.
  [[nodiscard]] std::string_view command();
  void set_command(std::string_view val);

  // Arguments that the command should be invoked with.
  [[nodiscard]] List<LSPAny> arguments();
  [[nodiscard]] bool has_arguments() const;
  [[nodiscard]] List<LSPAny> add_arguments();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] ExecuteCommandParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] ExecuteCommandParams::Union2 add_workDoneToken();

  void init();
};

/**
 * Parameters for a {@link DocumentColorRequest}.
 */
struct DocumentColorParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // An optional token that a server can use to report work done progress.
  struct Union1 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DocumentColorParams::Union1 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] DocumentColorParams::Union1 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DocumentColorParams::Union2 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] DocumentColorParams::Union2 add_partialResultToken();

  void init();
};

/**
 * Parameters for a {@link FoldingRangeRequest}.
 */
struct FoldingRangeParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // An optional token that a server can use to report work done progress.
  struct Union1 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] FoldingRangeParams::Union1 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] FoldingRangeParams::Union1 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] FoldingRangeParams::Union2 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] FoldingRangeParams::Union2 add_partialResultToken();

  void init();
};

/**
 * A parameter literal used in selection range requests.
 */
struct SelectionRangeParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The positions inside the text document.
  [[nodiscard]] List<Position> positions();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] SelectionRangeParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] SelectionRangeParams::Union2 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] SelectionRangeParams::Union3 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] SelectionRangeParams::Union3 add_partialResultToken();

  void init();
};

/**
 * @since 3.16.0
 */
struct SemanticTokensParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // An optional token that a server can use to report work done progress.
  struct Union1 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] SemanticTokensParams::Union1 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] SemanticTokensParams::Union1 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] SemanticTokensParams::Union2 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] SemanticTokensParams::Union2 add_partialResultToken();

  void init();
};

/**
 * @since 3.16.0
 */
struct SemanticTokensDeltaParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The result id of a previous response. The result Id can either point to a full response
  // or a delta response depending on what was received last.
  [[nodiscard]] std::string_view previousResultId();
  void set_previousResultId(std::string_view val);

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] SemanticTokensDeltaParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] SemanticTokensDeltaParams::Union2 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] SemanticTokensDeltaParams::Union3 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] SemanticTokensDeltaParams::Union3 add_partialResultToken();

  void init();
};

/**
 * Parameters of the document diagnostic request.
 * 
 * @since 3.17.0
 */
struct DocumentDiagnosticParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The additional identifier  provided during registration.
  [[nodiscard]] std::string_view identifier();
  [[nodiscard]] bool has_identifier() const;
  void set_identifier(std::string_view val);

  // The result id of a previous response if provided.
  [[nodiscard]] std::string_view previousResultId();
  [[nodiscard]] bool has_previousResultId() const;
  void set_previousResultId(std::string_view val);

  // An optional token that a server can use to report work done progress.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DocumentDiagnosticParams::Union3 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] DocumentDiagnosticParams::Union3 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union4 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DocumentDiagnosticParams::Union4 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] DocumentDiagnosticParams::Union4 add_partialResultToken();

  void init();
};

/**
 * Parameters of the workspace diagnostic request.
 * 
 * @since 3.17.0
 */
struct WorkspaceDiagnosticParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The additional identifier provided during registration.
  [[nodiscard]] std::string_view identifier();
  [[nodiscard]] bool has_identifier() const;
  void set_identifier(std::string_view val);

  // The currently known diagnostic reports with their
  // previous result ids.
  [[nodiscard]] List<PreviousResultId> previousResultIds();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] WorkspaceDiagnosticParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] WorkspaceDiagnosticParams::Union2 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] WorkspaceDiagnosticParams::Union3 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] WorkspaceDiagnosticParams::Union3 add_partialResultToken();

  void init();
};

/**
 * Parameters for a {@link DocumentSymbolRequest}.
 */
struct DocumentSymbolParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // An optional token that a server can use to report work done progress.
  struct Union1 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DocumentSymbolParams::Union1 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] DocumentSymbolParams::Union1 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DocumentSymbolParams::Union2 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] DocumentSymbolParams::Union2 add_partialResultToken();

  void init();
};

/**
 * The parameters of a {@link WorkspaceSymbolRequest}.
 */
struct WorkspaceSymbolParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A query string to filter symbols by. Clients may send an empty
  // string here to request all symbols.
  // 
  // The `query`-parameter should be interpreted in a *relaxed way* as editors
  // will apply their own highlighting and scoring on the results. A good rule
  // of thumb is to match case-insensitive and to simply check that the
  // characters of *query* appear in their order in a candidate symbol.
  // Servers shouldn't use prefix, substring, or similar strict matching.
  [[nodiscard]] std::string_view query();
  void set_query(std::string_view val);

  // An optional token that a server can use to report work done progress.
  struct Union1 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] WorkspaceSymbolParams::Union1 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] WorkspaceSymbolParams::Union1 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] WorkspaceSymbolParams::Union2 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] WorkspaceSymbolParams::Union2 add_partialResultToken();

  void init();
};

/**
 * The parameters of a {@link CodeLensRequest}.
 */
struct CodeLensParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document to request code lens for.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // An optional token that a server can use to report work done progress.
  struct Union1 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] CodeLensParams::Union1 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] CodeLensParams::Union1 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] CodeLensParams::Union2 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] CodeLensParams::Union2 add_partialResultToken();

  void init();
};

/**
 * The parameters of a {@link DocumentLinkRequest}.
 */
struct DocumentLinkParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document to provide document links for.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // An optional token that a server can use to report work done progress.
  struct Union1 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DocumentLinkParams::Union1 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] DocumentLinkParams::Union1 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DocumentLinkParams::Union2 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] DocumentLinkParams::Union2 add_partialResultToken();

  void init();
};

/**
 * Create file operation.
 */
struct CreateFile : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The resource to create.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  // Additional options
  [[nodiscard]] CreateFileOptions options();
  [[nodiscard]] bool has_options() const;
  [[nodiscard]] CreateFileOptions add_options();

  // An optional annotation identifier describing the operation.
  // 
  // @since 3.16.0
  [[nodiscard]] ChangeAnnotationIdentifier annotationId();
  [[nodiscard]] bool has_annotationId() const;
  void set_annotationId(ChangeAnnotationIdentifier val);

  void init();
};

/**
 * Rename file operation
 */
struct RenameFile : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The old (existing) location.
  [[nodiscard]] std::string_view oldUri();
  void set_oldUri(std::string_view val);

  // The new location.
  [[nodiscard]] std::string_view newUri();
  void set_newUri(std::string_view val);

  // Rename options.
  [[nodiscard]] RenameFileOptions options();
  [[nodiscard]] bool has_options() const;
  [[nodiscard]] RenameFileOptions add_options();

  // An optional annotation identifier describing the operation.
  // 
  // @since 3.16.0
  [[nodiscard]] ChangeAnnotationIdentifier annotationId();
  [[nodiscard]] bool has_annotationId() const;
  void set_annotationId(ChangeAnnotationIdentifier val);

  void init();
};

/**
 * Delete file operation
 */
struct DeleteFile : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The file to delete.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  // Delete options.
  [[nodiscard]] DeleteFileOptions options();
  [[nodiscard]] bool has_options() const;
  [[nodiscard]] DeleteFileOptions add_options();

  // An optional annotation identifier describing the operation.
  // 
  // @since 3.16.0
  [[nodiscard]] ChangeAnnotationIdentifier annotationId();
  [[nodiscard]] bool has_annotationId() const;
  void set_annotationId(ChangeAnnotationIdentifier val);

  void init();
};

/**
 * A notebook cell.
 * 
 * A cell's document URI must be unique across ALL notebook
 * cells and can therefore be used to uniquely identify a
 * notebook cell or the cell's text document.
 * 
 * @since 3.17.0
 */
struct NotebookCell : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The cell's kind
  [[nodiscard]] NotebookCellKind kind();
  void set_kind(NotebookCellKind val);

  // The URI of the cell's text document
  // content.
  [[nodiscard]] std::string_view document();
  void set_document(std::string_view val);

  // Additional metadata stored with the cell.
  // 
  // Note: should always be an object literal (e.g. LSPObject)
  [[nodiscard]] LSPObject metadata();
  [[nodiscard]] bool has_metadata() const;
  [[nodiscard]] LSPObject add_metadata();

  // Additional execution summary information
  // if supported by the client.
  [[nodiscard]] ExecutionSummary executionSummary();
  [[nodiscard]] bool has_executionSummary() const;
  [[nodiscard]] ExecutionSummary add_executionSummary();

  void init();
};

/**
 * Documentation for a class of code actions.
 * 
 * @since 3.18.0
 * @proposed
 */
struct CodeActionKindDocumentation : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The kind of the code action being documented.
  // 
  // If the kind is generic, such as `CodeActionKind.Refactor`, the documentation will be shown whenever any
  // refactorings are returned. If the kind if more specific, such as `CodeActionKind.RefactorExtract`, the
  // documentation will only be shown when extract refactoring code actions are returned.
  [[nodiscard]] CodeActionKind kind();
  void set_kind(CodeActionKind val);

  // Command that is ued to display the documentation to the user.
  // 
  // The title of this documentation code action is taken from {@linkcode Command.title}
  [[nodiscard]] Command command();

  void init();
};

struct RegistrationParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] List<Registration> registrations();

  void init();
};

/**
 * General client capabilities.
 * 
 * @since 3.16.0
 */
struct GeneralClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Client capability that signals how the client
  // handles stale requests (e.g. a request
  // for which the client will not process the response
  // anymore since the information is outdated).
  // 
  // @since 3.17.0
  [[nodiscard]] StaleRequestSupportOptions staleRequestSupport();
  [[nodiscard]] bool has_staleRequestSupport() const;
  [[nodiscard]] StaleRequestSupportOptions add_staleRequestSupport();

  // Client capabilities specific to regular expressions.
  // 
  // @since 3.16.0
  [[nodiscard]] RegularExpressionsClientCapabilities regularExpressions();
  [[nodiscard]] bool has_regularExpressions() const;
  [[nodiscard]] RegularExpressionsClientCapabilities add_regularExpressions();

  // Client capabilities specific to the client's markdown parser.
  // 
  // @since 3.16.0
  [[nodiscard]] MarkdownClientCapabilities markdown();
  [[nodiscard]] bool has_markdown() const;
  [[nodiscard]] MarkdownClientCapabilities add_markdown();

  // The position encodings supported by the client. Client and server
  // have to agree on the same position encoding to ensure that offsets
  // (e.g. character position in a line) are interpreted the same on both
  // sides.
  // 
  // To keep the protocol backwards compatible the following applies: if
  // the value 'utf-16' is missing from the array of position encodings
  // servers can assume that the client supports UTF-16. UTF-16 is
  // therefore a mandatory encoding.
  // 
  // If omitted it defaults to ['utf-16'].
  // 
  // Implementation considerations: since the conversion from one encoding
  // into another requires the content of the file / line the conversion
  // is best done where the file is read which is usually on the server
  // side.
  // 
  // @since 3.17.0
  [[nodiscard]] List<PositionEncodingKind> positionEncodings();
  [[nodiscard]] bool has_positionEncodings() const;
  [[nodiscard]] List<PositionEncodingKind> add_positionEncodings();

  void init();
};

/**
 * The parameters of a `workspace/didChangeWorkspaceFolders` notification.
 */
struct DidChangeWorkspaceFoldersParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The actual workspace folder change event.
  [[nodiscard]] WorkspaceFoldersChangeEvent event();

  void init();
};

/**
 * The glob pattern. Either a string pattern or a relative pattern.
 * 
 * @since 3.17.0
 */
struct GlobPattern : detail::StructWrapper {
  using StructWrapper::StructWrapper;

};

/**
 * Registration options for a {@link WorkspaceSymbolRequest}.
 */
struct WorkspaceSymbolRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The server provides support to resolve additional
  // information for a workspace symbol.
  // 
  // @since 3.17.0
  [[nodiscard]] bool resolveProvider();
  [[nodiscard]] bool has_resolveProvider() const;
  void set_resolveProvider(bool val);

  void init();
};

/**
 * Registration options for a {@link ExecuteCommandRequest}.
 */
struct ExecuteCommandRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The commands to be executed on the server
  [[nodiscard]] List<std::string_view> commands();

  void init();
};

struct ImplementationParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] ImplementationParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] ImplementationParams::Union2 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] ImplementationParams::Union3 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] ImplementationParams::Union3 add_partialResultToken();

  void init();
};

struct TypeDefinitionParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] TypeDefinitionParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] TypeDefinitionParams::Union2 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] TypeDefinitionParams::Union3 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] TypeDefinitionParams::Union3 add_partialResultToken();

  void init();
};

struct DeclarationParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DeclarationParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] DeclarationParams::Union2 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DeclarationParams::Union3 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] DeclarationParams::Union3 add_partialResultToken();

  void init();
};

/**
 * The parameter of a `textDocument/prepareCallHierarchy` request.
 * 
 * @since 3.16.0
 */
struct CallHierarchyPrepareParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] CallHierarchyPrepareParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] CallHierarchyPrepareParams::Union2 add_workDoneToken();

  void init();
};

struct LinkedEditingRangeParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] LinkedEditingRangeParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] LinkedEditingRangeParams::Union2 add_workDoneToken();

  void init();
};

struct MonikerParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] MonikerParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] MonikerParams::Union2 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] MonikerParams::Union3 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] MonikerParams::Union3 add_partialResultToken();

  void init();
};

/**
 * The parameter of a `textDocument/prepareTypeHierarchy` request.
 * 
 * @since 3.17.0
 */
struct TypeHierarchyPrepareParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] TypeHierarchyPrepareParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] TypeHierarchyPrepareParams::Union2 add_workDoneToken();

  void init();
};

/**
 * Completion parameters
 */
struct CompletionParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The completion context. This is only available it the client specifies
  // to send this using the client capability `textDocument.completion.contextSupport === true`
  [[nodiscard]] CompletionContext context();
  [[nodiscard]] bool has_context() const;
  [[nodiscard]] CompletionContext add_context();

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  // An optional token that a server can use to report work done progress.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] CompletionParams::Union3 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] CompletionParams::Union3 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union4 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] CompletionParams::Union4 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] CompletionParams::Union4 add_partialResultToken();

  void init();
};

/**
 * Parameters for a {@link HoverRequest}.
 */
struct HoverParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] HoverParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] HoverParams::Union2 add_workDoneToken();

  void init();
};

/**
 * Parameters for a {@link DefinitionRequest}.
 */
struct DefinitionParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DefinitionParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] DefinitionParams::Union2 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DefinitionParams::Union3 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] DefinitionParams::Union3 add_partialResultToken();

  void init();
};

/**
 * Parameters for a {@link ReferencesRequest}.
 */
struct ReferenceParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] ReferenceContext context();

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  // An optional token that a server can use to report work done progress.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] ReferenceParams::Union3 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] ReferenceParams::Union3 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union4 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] ReferenceParams::Union4 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] ReferenceParams::Union4 add_partialResultToken();

  void init();
};

/**
 * Parameters for a {@link DocumentHighlightRequest}.
 */
struct DocumentHighlightParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DocumentHighlightParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] DocumentHighlightParams::Union2 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DocumentHighlightParams::Union3 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] DocumentHighlightParams::Union3 add_partialResultToken();

  void init();
};

struct PrepareRenameParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] PrepareRenameParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] PrepareRenameParams::Union2 add_workDoneToken();

  void init();
};

/**
 * Represents a location inside a resource, such as a line
 * inside a text file.
 */
struct Location : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);
  [[nodiscard]] Range range();

  void init();
};

/**
 * Represents a color range from a document.
 */
struct ColorInformation : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The range in the document where this color appears.
  [[nodiscard]] Range range();

  // The actual color value for this color range.
  [[nodiscard]] Color color();

  void init();
};

/**
 * Parameters for a {@link ColorPresentationRequest}.
 */
struct ColorPresentationParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The color to request presentations for.
  [[nodiscard]] Color color();

  // The range where the color would be inserted. Serves as a context.
  [[nodiscard]] Range range();

  // An optional token that a server can use to report work done progress.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] ColorPresentationParams::Union3 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] ColorPresentationParams::Union3 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union4 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] ColorPresentationParams::Union4 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] ColorPresentationParams::Union4 add_partialResultToken();

  void init();
};

/**
 * A selection range represents a part of a selection hierarchy. A selection range
 * may have a parent selection range that contains it.
 */
struct SelectionRange : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The {@link Range range} of this selection range.
  [[nodiscard]] Range range();

  // The parent selection range containing this range. Therefore `parent.range` must contain `this.range`.
  [[nodiscard]] SelectionRange parent();
  [[nodiscard]] bool has_parent() const;
  [[nodiscard]] SelectionRange add_parent();

  void init();
};

/**
 * Represents programming constructs like functions or constructors in the context
 * of call hierarchy.
 * 
 * @since 3.16.0
 */
struct CallHierarchyItem : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The name of this item.
  [[nodiscard]] std::string_view name();
  void set_name(std::string_view val);

  // The kind of this item.
  [[nodiscard]] SymbolKind kind();
  void set_kind(SymbolKind val);

  // Tags for this item.
  [[nodiscard]] List<SymbolTag> tags();
  [[nodiscard]] bool has_tags() const;
  [[nodiscard]] List<SymbolTag> add_tags();

  // More detail for this item, e.g. the signature of a function.
  [[nodiscard]] std::string_view detail();
  [[nodiscard]] bool has_detail() const;
  void set_detail(std::string_view val);

  // The resource identifier of this item.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  // The range enclosing this symbol not including leading/trailing whitespace but everything else, e.g. comments and code.
  [[nodiscard]] Range range();

  // The range that should be selected and revealed when this symbol is being picked, e.g. the name of a function.
  // Must be contained by the {@link CallHierarchyItem.range `range`}.
  [[nodiscard]] Range selectionRange();

  // A data entry field that is preserved between a call hierarchy prepare and
  // incoming calls or outgoing calls requests.
  struct Union7 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] CallHierarchyItem::Union7 data();
  [[nodiscard]] bool has_data() const;
  [[nodiscard]] CallHierarchyItem::Union7 add_data();

  void init();
};

/**
 * @since 3.16.0
 */
struct SemanticTokensRangeParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The range the semantic tokens are requested for.
  [[nodiscard]] Range range();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] SemanticTokensRangeParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] SemanticTokensRangeParams::Union2 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] SemanticTokensRangeParams::Union3 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] SemanticTokensRangeParams::Union3 add_partialResultToken();

  void init();
};

/**
 * Params to show a resource in the UI.
 * 
 * @since 3.16.0
 */
struct ShowDocumentParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The uri to show.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  // Indicates to show the resource in an external program.
  // To show, for example, `https://code.visualstudio.com/`
  // in the default WEB browser set `external` to `true`.
  [[nodiscard]] bool external();
  [[nodiscard]] bool has_external() const;
  void set_external(bool val);

  // An optional property to indicate whether the editor
  // showing the document should take focus or not.
  // Clients might ignore this property if an external
  // program is started.
  [[nodiscard]] bool takeFocus();
  [[nodiscard]] bool has_takeFocus() const;
  void set_takeFocus(bool val);

  // An optional selection range if the document is a text
  // document. Clients might ignore the property if an
  // external program is started or the file is not a text
  // file.
  [[nodiscard]] Range selection();
  [[nodiscard]] bool has_selection() const;
  [[nodiscard]] Range add_selection();

  void init();
};

/**
 * The result of a linked editing range request.
 * 
 * @since 3.16.0
 */
struct LinkedEditingRanges : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A list of ranges that can be edited together. The ranges must have
  // identical length and contain identical text content. The ranges cannot overlap.
  [[nodiscard]] List<Range> ranges();

  // An optional word pattern (regular expression) that describes valid contents for
  // the given ranges. If no pattern is provided, the client configuration's word
  // pattern will be used.
  [[nodiscard]] std::string_view wordPattern();
  [[nodiscard]] bool has_wordPattern() const;
  void set_wordPattern(std::string_view val);

  void init();
};

/**
 * @since 3.17.0
 */
struct TypeHierarchyItem : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The name of this item.
  [[nodiscard]] std::string_view name();
  void set_name(std::string_view val);

  // The kind of this item.
  [[nodiscard]] SymbolKind kind();
  void set_kind(SymbolKind val);

  // Tags for this item.
  [[nodiscard]] List<SymbolTag> tags();
  [[nodiscard]] bool has_tags() const;
  [[nodiscard]] List<SymbolTag> add_tags();

  // More detail for this item, e.g. the signature of a function.
  [[nodiscard]] std::string_view detail();
  [[nodiscard]] bool has_detail() const;
  void set_detail(std::string_view val);

  // The resource identifier of this item.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  // The range enclosing this symbol not including leading/trailing whitespace
  // but everything else, e.g. comments and code.
  [[nodiscard]] Range range();

  // The range that should be selected and revealed when this symbol is being
  // picked, e.g. the name of a function. Must be contained by the
  // {@link TypeHierarchyItem.range `range`}.
  [[nodiscard]] Range selectionRange();

  // A data entry field that is preserved between a type hierarchy prepare and
  // supertypes or subtypes requests. It could also be used to identify the
  // type hierarchy in the server, helping improve the performance on
  // resolving supertypes and subtypes.
  struct Union7 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] TypeHierarchyItem::Union7 data();
  [[nodiscard]] bool has_data() const;
  [[nodiscard]] TypeHierarchyItem::Union7 add_data();

  void init();
};

/**
 * A parameter literal used in inlay hint requests.
 * 
 * @since 3.17.0
 */
struct InlayHintParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The document range for which inlay hints should be computed.
  [[nodiscard]] Range range();

  // An optional token that a server can use to report work done progress.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] InlayHintParams::Union2 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] InlayHintParams::Union2 add_workDoneToken();

  void init();
};

/**
 * An inline completion item represents a text snippet that is proposed inline to complete text that is being typed.
 * 
 * @since 3.18.0
 * @proposed
 */
struct InlineCompletionItem : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text to replace the range with. Must be set.
  struct Union0 : detail::UnionStructWrapper<std::string_view, StringValue> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_StringValue() const;
    [[nodiscard]] StringValue as_StringValue();

  };

  [[nodiscard]] InlineCompletionItem::Union0 insertText();

  // A text that is used to decide if this inline completion should be shown. When `falsy` the {@link InlineCompletionItem.insertText} is used.
  [[nodiscard]] std::string_view filterText();
  [[nodiscard]] bool has_filterText() const;
  void set_filterText(std::string_view val);

  // The range to replace. Must begin and end on the same line.
  [[nodiscard]] Range range();
  [[nodiscard]] bool has_range() const;
  [[nodiscard]] Range add_range();

  // An optional {@link Command} that is executed *after* inserting this completion.
  [[nodiscard]] Command command();
  [[nodiscard]] bool has_command() const;
  [[nodiscard]] Command add_command();

  void init();
};

/**
 * A text edit applicable to a text document.
 */
struct TextEdit : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The range of the text document to be manipulated. To insert
  // text into a document create a range where start === end.
  [[nodiscard]] Range range();

  // The string to be inserted. For delete operations use an
  // empty string.
  [[nodiscard]] std::string_view newText();
  void set_newText(std::string_view val);

  void init();
};

/**
 * The result of a hover request.
 */
struct Hover : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The hover's content
  struct Union0 : detail::UnionStructWrapper<MarkupContent, MarkedString, List<MarkedString>> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_MarkupContent() const;
    [[nodiscard]] MarkupContent as_MarkupContent();

    struct Union1 : detail::UnionStructWrapper<std::string_view, MarkedStringWithLanguage> {
      using UnionStructWrapper::UnionStructWrapper;

      [[nodiscard]] bool holds_string() const;
      [[nodiscard]] std::string_view as_string();
      void set_string(std::string_view val);

      [[nodiscard]] bool holds_MarkedStringWithLanguage() const;
      [[nodiscard]] MarkedStringWithLanguage as_MarkedStringWithLanguage();

    };

    [[nodiscard]] bool holds_reference_Union1() const;
    [[nodiscard]] MarkedString as_reference_Union1();

    [[nodiscard]] bool holds_array_MarkedString() const;
    [[nodiscard]] List<MarkedString> as_array_MarkedString();

  };

  [[nodiscard]] Hover::Union0 contents();

  // An optional range inside the text document that is used to
  // visualize the hover, e.g. by changing the background color.
  [[nodiscard]] Range range();
  [[nodiscard]] bool has_range() const;
  [[nodiscard]] Range add_range();

  void init();
};

/**
 * A document highlight is a range inside a text document which deserves
 * special attention. Usually a document highlight is visualized by changing
 * the background color of its range.
 */
struct DocumentHighlight : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The range this highlight applies to.
  [[nodiscard]] Range range();

  // The highlight kind, default is {@link DocumentHighlightKind.Text text}.
  [[nodiscard]] DocumentHighlightKind kind();
  [[nodiscard]] bool has_kind() const;
  void set_kind(DocumentHighlightKind val);

  void init();
};

/**
 * Represents programming constructs like variables, classes, interfaces etc.
 * that appear in a document. Document symbols can be hierarchical and they
 * have two ranges: one that encloses its definition and one that points to
 * its most interesting range, e.g. the range of an identifier.
 */
struct DocumentSymbol : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The name of this symbol. Will be displayed in the user interface and therefore must not be
  // an empty string or a string only consisting of white spaces.
  [[nodiscard]] std::string_view name();
  void set_name(std::string_view val);

  // More detail for this symbol, e.g the signature of a function.
  [[nodiscard]] std::string_view detail();
  [[nodiscard]] bool has_detail() const;
  void set_detail(std::string_view val);

  // The kind of this symbol.
  [[nodiscard]] SymbolKind kind();
  void set_kind(SymbolKind val);

  // Tags for this document symbol.
  // 
  // @since 3.16.0
  [[nodiscard]] List<SymbolTag> tags();
  [[nodiscard]] bool has_tags() const;
  [[nodiscard]] List<SymbolTag> add_tags();

  // Indicates if this symbol is deprecated.
  // 
  // @deprecated Use tags instead
  [[nodiscard]] bool deprecated();
  [[nodiscard]] bool has_deprecated() const;
  void set_deprecated(bool val);

  // The range enclosing this symbol not including leading/trailing whitespace but everything else
  // like comments. This information is typically used to determine if the clients cursor is
  // inside the symbol to reveal in the symbol in the UI.
  [[nodiscard]] Range range();

  // The range that should be selected and revealed when this symbol is being picked, e.g the name of a function.
  // Must be contained by the `range`.
  [[nodiscard]] Range selectionRange();

  // Children of this symbol, e.g. properties of a class.
  [[nodiscard]] List<DocumentSymbol> children();
  [[nodiscard]] bool has_children() const;
  [[nodiscard]] List<DocumentSymbol> add_children();

  void init();
};

/**
 * A code lens represents a {@link Command command} that should be shown along with
 * source text, like the number of references, a way to run tests, etc.
 * 
 * A code lens is _unresolved_ when no command is associated to it. For performance
 * reasons the creation of a code lens and resolving should be done in two stages.
 */
struct CodeLens : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The range in which this code lens is valid. Should only span a single line.
  [[nodiscard]] Range range();

  // The command this code lens represents.
  [[nodiscard]] Command command();
  [[nodiscard]] bool has_command() const;
  [[nodiscard]] Command add_command();

  // A data entry field that is preserved on a code lens item between
  // a {@link CodeLensRequest} and a {@link CodeLensResolveRequest}
  struct Union2 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] CodeLens::Union2 data();
  [[nodiscard]] bool has_data() const;
  [[nodiscard]] CodeLens::Union2 add_data();

  void init();
};

/**
 * A document link is a range in a text document that links to an internal or external resource, like another
 * text document or a web site.
 */
struct DocumentLink : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The range this link applies to.
  [[nodiscard]] Range range();

  // The uri this link points to. If missing a resolve request is sent later.
  [[nodiscard]] std::string_view target();
  [[nodiscard]] bool has_target() const;
  void set_target(std::string_view val);

  // The tooltip text when you hover over this link.
  // 
  // If a tooltip is provided, is will be displayed in a string that includes instructions on how to
  // trigger the link, such as `{0} (ctrl + click)`. The specific instructions vary depending on OS,
  // user settings, and localization.
  // 
  // @since 3.15.0
  [[nodiscard]] std::string_view tooltip();
  [[nodiscard]] bool has_tooltip() const;
  void set_tooltip(std::string_view val);

  // A data entry field that is preserved on a document link between a
  // DocumentLinkRequest and a DocumentLinkResolveRequest.
  struct Union3 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] DocumentLink::Union3 data();
  [[nodiscard]] bool has_data() const;
  [[nodiscard]] DocumentLink::Union3 add_data();

  void init();
};

/**
 * The parameters of a {@link DocumentRangeFormattingRequest}.
 */
struct DocumentRangeFormattingParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document to format.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The range to format
  [[nodiscard]] Range range();

  // The format options
  [[nodiscard]] FormattingOptions options();

  // An optional token that a server can use to report work done progress.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DocumentRangeFormattingParams::Union3 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] DocumentRangeFormattingParams::Union3 add_workDoneToken();

  void init();
};

/**
 * The parameters of a {@link DocumentRangesFormattingRequest}.
 * 
 * @since 3.18.0
 * @proposed
 */
struct DocumentRangesFormattingParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document to format.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The ranges to format
  [[nodiscard]] List<Range> ranges();

  // The format options
  [[nodiscard]] FormattingOptions options();

  // An optional token that a server can use to report work done progress.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] DocumentRangesFormattingParams::Union3 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] DocumentRangesFormattingParams::Union3 add_workDoneToken();

  void init();
};

/**
 * Represents the connection of two locations. Provides additional metadata over normal {@link Location locations},
 * including an origin range.
 */
struct LocationLink : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Span of the origin of this link.
  // 
  // Used as the underlined span for mouse interaction. Defaults to the word range at
  // the definition position.
  [[nodiscard]] Range originSelectionRange();
  [[nodiscard]] bool has_originSelectionRange() const;
  [[nodiscard]] Range add_originSelectionRange();

  // The target resource identifier of this link.
  [[nodiscard]] std::string_view targetUri();
  void set_targetUri(std::string_view val);

  // The full target range of this link. If the target for example is a symbol then target range is the
  // range enclosing this symbol not including leading/trailing whitespace but everything else
  // like comments. This information is typically used to highlight the range in the editor.
  [[nodiscard]] Range targetRange();

  // The range that should be selected and revealed when this link is being followed, e.g the name of a function.
  // Must be contained by the `targetRange`. See also `DocumentSymbol#range`
  [[nodiscard]] Range targetSelectionRange();

  void init();
};

/**
 * @since 3.17.0
 */
struct InlineValueContext : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The stack frame (as a DAP Id) where the execution has stopped.
  [[nodiscard]] std::int32_t frameId();
  void set_frameId(std::int32_t val);

  // The document range where execution has stopped.
  // Typically the end position of the range denotes the line where the inline values are shown.
  [[nodiscard]] Range stoppedLocation();

  void init();
};

/**
 * Provide inline value as text.
 * 
 * @since 3.17.0
 */
struct InlineValueText : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document range for which the inline value applies.
  [[nodiscard]] Range range();

  // The text of the inline value.
  [[nodiscard]] std::string_view text();
  void set_text(std::string_view val);

  void init();
};

/**
 * Provide inline value through a variable lookup.
 * If only a range is specified, the variable name will be extracted from the underlying document.
 * An optional variable name can be used to override the extracted name.
 * 
 * @since 3.17.0
 */
struct InlineValueVariableLookup : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document range for which the inline value applies.
  // The range is used to extract the variable name from the underlying document.
  [[nodiscard]] Range range();

  // If specified the name of the variable to look up.
  [[nodiscard]] std::string_view variableName();
  [[nodiscard]] bool has_variableName() const;
  void set_variableName(std::string_view val);

  // How to perform the lookup.
  [[nodiscard]] bool caseSensitiveLookup();
  void set_caseSensitiveLookup(bool val);

  void init();
};

/**
 * Provide an inline value through an expression evaluation.
 * If only a range is specified, the expression will be extracted from the underlying document.
 * An optional expression can be used to override the extracted expression.
 * 
 * @since 3.17.0
 */
struct InlineValueEvaluatableExpression : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document range for which the inline value applies.
  // The range is used to extract the evaluatable expression from the underlying document.
  [[nodiscard]] Range range();

  // If specified the expression overrides the extracted expression.
  [[nodiscard]] std::string_view expression();
  [[nodiscard]] bool has_expression() const;
  void set_expression(std::string_view val);

  void init();
};

/**
 * A special text edit to provide an insert and a replace operation.
 * 
 * @since 3.16.0
 */
struct InsertReplaceEdit : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The string to be inserted.
  [[nodiscard]] std::string_view newText();
  void set_newText(std::string_view val);

  // The range if the insert is requested
  [[nodiscard]] Range insert();

  // The range if the replace is requested.
  [[nodiscard]] Range replace();

  void init();
};

/**
 * @since 3.18.0
 */
struct PrepareRenamePlaceholder : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] Range range();
  [[nodiscard]] std::string_view placeholder();
  void set_placeholder(std::string_view val);

  void init();
};

/**
 * An interactive text edit.
 * 
 * @since 3.18.0
 * @proposed
 */
struct SnippetTextEdit : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The range of the text document to be manipulated.
  [[nodiscard]] Range range();

  // The snippet to be inserted.
  [[nodiscard]] StringValue snippet();

  // The actual identifier of the snippet edit.
  [[nodiscard]] ChangeAnnotationIdentifier annotationId();
  [[nodiscard]] bool has_annotationId() const;
  void set_annotationId(ChangeAnnotationIdentifier val);

  void init();
};

/**
 * Describes the currently selected completion item.
 * 
 * @since 3.18.0
 * @proposed
 */
struct SelectedCompletionInfo : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The range that will be replaced if this completion item is accepted.
  [[nodiscard]] Range range();

  // The text the range will be replaced with if this completion is accepted.
  [[nodiscard]] std::string_view text();
  void set_text(std::string_view val);

  void init();
};

/**
 * @since 3.18.0
 */
struct TextDocumentContentChangePartial : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The range of the document that changed.
  [[nodiscard]] Range range();

  // The optional length of the range that got replaced.
  // 
  // @deprecated use range instead.
  [[nodiscard]] std::uint32_t rangeLength();
  [[nodiscard]] bool has_rangeLength() const;
  void set_rangeLength(std::uint32_t val);

  // The new text for the provided range.
  [[nodiscard]] std::string_view text();
  void set_text(std::string_view val);

  void init();
};

/**
 * Edit range variant that includes ranges for insert and replace operations.
 * 
 * @since 3.18.0
 */
struct EditRangeWithInsertReplace : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] Range insert();
  [[nodiscard]] Range replace();

  void init();
};

/**
 * A filter to describe in which file operation requests or notifications
 * the server is interested in receiving.
 * 
 * @since 3.16.0
 */
struct FileOperationFilter : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A Uri scheme like `file` or `untitled`.
  [[nodiscard]] std::string_view scheme();
  [[nodiscard]] bool has_scheme() const;
  void set_scheme(std::string_view val);

  // The actual file operation pattern.
  [[nodiscard]] FileOperationPattern pattern();

  void init();
};

struct WindowClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // It indicates whether the client supports server initiated
  // progress using the `window/workDoneProgress/create` request.
  // 
  // The capability also controls Whether client supports handling
  // of progress notifications. If set servers are allowed to report a
  // `workDoneProgress` property in the request specific server
  // capabilities.
  // 
  // @since 3.15.0
  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  // Capabilities specific to the showMessage request.
  // 
  // @since 3.16.0
  [[nodiscard]] ShowMessageRequestClientCapabilities showMessage();
  [[nodiscard]] bool has_showMessage() const;
  [[nodiscard]] ShowMessageRequestClientCapabilities add_showMessage();

  // Capabilities specific to the showDocument request.
  // 
  // @since 3.16.0
  [[nodiscard]] ShowDocumentClientCapabilities showDocument();
  [[nodiscard]] bool has_showDocument() const;
  [[nodiscard]] ShowDocumentClientCapabilities add_showDocument();

  void init();
};

/**
 * Client Capabilities for a {@link SignatureHelpRequest}.
 */
struct SignatureHelpClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether signature help supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // The client supports the following `SignatureInformation`
  // specific properties.
  [[nodiscard]] ClientSignatureInformationOptions signatureInformation();
  [[nodiscard]] bool has_signatureInformation() const;
  [[nodiscard]] ClientSignatureInformationOptions add_signatureInformation();

  // The client supports to send additional context information for a
  // `textDocument/signatureHelp` request. A client that opts into
  // contextSupport will also support the `retriggerCharacters` on
  // `SignatureHelpOptions`.
  // 
  // @since 3.15.0
  [[nodiscard]] bool contextSupport();
  [[nodiscard]] bool has_contextSupport() const;
  void set_contextSupport(bool val);

  void init();
};

/**
 * @since 3.16.0
 */
struct SemanticTokensClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)`
  // return value for the corresponding server capability as well.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // Which requests the client supports and might send to the server
  // depending on the server's capability. Please note that clients might not
  // show semantic tokens or degrade some of the user experience if a range
  // or full request is advertised by the client but not provided by the
  // server. If for example the client capability `requests.full` and
  // `request.range` are both set to true but the server only provides a
  // range provider the client might not render a minimap correctly or might
  // even decide to not show any semantic tokens at all.
  [[nodiscard]] ClientSemanticTokensRequestOptions requests();

  // The token types that the client supports.
  [[nodiscard]] List<std::string_view> tokenTypes();

  // The token modifiers that the client supports.
  [[nodiscard]] List<std::string_view> tokenModifiers();

  // The token formats the clients supports.
  [[nodiscard]] List<TokenFormat> formats();

  // Whether the client supports tokens that can overlap each other.
  [[nodiscard]] bool overlappingTokenSupport();
  [[nodiscard]] bool has_overlappingTokenSupport() const;
  void set_overlappingTokenSupport(bool val);

  // Whether the client supports tokens that can span multiple lines.
  [[nodiscard]] bool multilineTokenSupport();
  [[nodiscard]] bool has_multilineTokenSupport() const;
  void set_multilineTokenSupport(bool val);

  // Whether the client allows the server to actively cancel a
  // semantic token request, e.g. supports returning
  // LSPErrorCodes.ServerCancelled. If a server does the client
  // needs to retrigger the request.
  // 
  // @since 3.17.0
  [[nodiscard]] bool serverCancelSupport();
  [[nodiscard]] bool has_serverCancelSupport() const;
  void set_serverCancelSupport(bool val);

  // Whether the client uses semantic tokens to augment existing
  // syntax tokens. If set to `true` client side created syntax
  // tokens and semantic tokens are both used for colorization. If
  // set to `false` the client only uses the returned semantic tokens
  // for colorization.
  // 
  // If the value is `undefined` then the client behavior is not
  // specified.
  // 
  // @since 3.17.0
  [[nodiscard]] bool augmentsSyntaxTokens();
  [[nodiscard]] bool has_augmentsSyntaxTokens() const;
  void set_augmentsSyntaxTokens(bool val);

  void init();
};

/**
 * Workspace specific client capabilities.
 */
struct WorkspaceClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The client supports applying batch edits
  // to the workspace by supporting the request
  // 'workspace/applyEdit'
  [[nodiscard]] bool applyEdit();
  [[nodiscard]] bool has_applyEdit() const;
  void set_applyEdit(bool val);

  // Capabilities specific to `WorkspaceEdit`s.
  [[nodiscard]] WorkspaceEditClientCapabilities workspaceEdit();
  [[nodiscard]] bool has_workspaceEdit() const;
  [[nodiscard]] WorkspaceEditClientCapabilities add_workspaceEdit();

  // Capabilities specific to the `workspace/didChangeConfiguration` notification.
  [[nodiscard]] DidChangeConfigurationClientCapabilities didChangeConfiguration();
  [[nodiscard]] bool has_didChangeConfiguration() const;
  [[nodiscard]] DidChangeConfigurationClientCapabilities add_didChangeConfiguration();

  // Capabilities specific to the `workspace/didChangeWatchedFiles` notification.
  [[nodiscard]] DidChangeWatchedFilesClientCapabilities didChangeWatchedFiles();
  [[nodiscard]] bool has_didChangeWatchedFiles() const;
  [[nodiscard]] DidChangeWatchedFilesClientCapabilities add_didChangeWatchedFiles();

  // Capabilities specific to the `workspace/symbol` request.
  [[nodiscard]] WorkspaceSymbolClientCapabilities symbol();
  [[nodiscard]] bool has_symbol() const;
  [[nodiscard]] WorkspaceSymbolClientCapabilities add_symbol();

  // Capabilities specific to the `workspace/executeCommand` request.
  [[nodiscard]] ExecuteCommandClientCapabilities executeCommand();
  [[nodiscard]] bool has_executeCommand() const;
  [[nodiscard]] ExecuteCommandClientCapabilities add_executeCommand();

  // The client has support for workspace folders.
  // 
  // @since 3.6.0
  [[nodiscard]] bool workspaceFolders();
  [[nodiscard]] bool has_workspaceFolders() const;
  void set_workspaceFolders(bool val);

  // The client supports `workspace/configuration` requests.
  // 
  // @since 3.6.0
  [[nodiscard]] bool configuration();
  [[nodiscard]] bool has_configuration() const;
  void set_configuration(bool val);

  // Capabilities specific to the semantic token requests scoped to the
  // workspace.
  // 
  // @since 3.16.0.
  [[nodiscard]] SemanticTokensWorkspaceClientCapabilities semanticTokens();
  [[nodiscard]] bool has_semanticTokens() const;
  [[nodiscard]] SemanticTokensWorkspaceClientCapabilities add_semanticTokens();

  // Capabilities specific to the code lens requests scoped to the
  // workspace.
  // 
  // @since 3.16.0.
  [[nodiscard]] CodeLensWorkspaceClientCapabilities codeLens();
  [[nodiscard]] bool has_codeLens() const;
  [[nodiscard]] CodeLensWorkspaceClientCapabilities add_codeLens();

  // The client has support for file notifications/requests for user operations on files.
  // 
  // Since 3.16.0
  [[nodiscard]] FileOperationClientCapabilities fileOperations();
  [[nodiscard]] bool has_fileOperations() const;
  [[nodiscard]] FileOperationClientCapabilities add_fileOperations();

  // Capabilities specific to the inline values requests scoped to the
  // workspace.
  // 
  // @since 3.17.0.
  [[nodiscard]] InlineValueWorkspaceClientCapabilities inlineValue();
  [[nodiscard]] bool has_inlineValue() const;
  [[nodiscard]] InlineValueWorkspaceClientCapabilities add_inlineValue();

  // Capabilities specific to the inlay hint requests scoped to the
  // workspace.
  // 
  // @since 3.17.0.
  [[nodiscard]] InlayHintWorkspaceClientCapabilities inlayHint();
  [[nodiscard]] bool has_inlayHint() const;
  [[nodiscard]] InlayHintWorkspaceClientCapabilities add_inlayHint();

  // Capabilities specific to the diagnostic requests scoped to the
  // workspace.
  // 
  // @since 3.17.0.
  [[nodiscard]] DiagnosticWorkspaceClientCapabilities diagnostics();
  [[nodiscard]] bool has_diagnostics() const;
  [[nodiscard]] DiagnosticWorkspaceClientCapabilities add_diagnostics();

  // Capabilities specific to the folding range requests scoped to the workspace.
  // 
  // @since 3.18.0
  // @proposed
  [[nodiscard]] FoldingRangeWorkspaceClientCapabilities foldingRange();
  [[nodiscard]] bool has_foldingRange() const;
  [[nodiscard]] FoldingRangeWorkspaceClientCapabilities add_foldingRange();

  // Capabilities specific to the `workspace/textDocumentContent` request.
  // 
  // @since 3.18.0
  // @proposed
  [[nodiscard]] TextDocumentContentClientCapabilities textDocumentContent();
  [[nodiscard]] bool has_textDocumentContent() const;
  [[nodiscard]] TextDocumentContentClientCapabilities add_textDocumentContent();

  void init();
};

/**
 * Completion client capabilities
 */
struct CompletionClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether completion supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // The client supports the following `CompletionItem` specific
  // capabilities.
  [[nodiscard]] ClientCompletionItemOptions completionItem();
  [[nodiscard]] bool has_completionItem() const;
  [[nodiscard]] ClientCompletionItemOptions add_completionItem();
  [[nodiscard]] ClientCompletionItemOptionsKind completionItemKind();
  [[nodiscard]] bool has_completionItemKind() const;
  [[nodiscard]] ClientCompletionItemOptionsKind add_completionItemKind();

  // Defines how the client handles whitespace and indentation
  // when accepting a completion item that uses multi line
  // text in either `insertText` or `textEdit`.
  // 
  // @since 3.17.0
  [[nodiscard]] InsertTextMode insertTextMode();
  [[nodiscard]] bool has_insertTextMode() const;
  void set_insertTextMode(InsertTextMode val);

  // The client supports to send additional context information for a
  // `textDocument/completion` request.
  [[nodiscard]] bool contextSupport();
  [[nodiscard]] bool has_contextSupport() const;
  void set_contextSupport(bool val);

  // The client supports the following `CompletionList` specific
  // capabilities.
  // 
  // @since 3.17.0
  [[nodiscard]] CompletionListCapabilities completionList();
  [[nodiscard]] bool has_completionList() const;
  [[nodiscard]] CompletionListCapabilities add_completionList();

  void init();
};

/**
 * The Client Capabilities of a {@link CodeActionRequest}.
 */
struct CodeActionClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether code action supports dynamic registration.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // The client support code action literals of type `CodeAction` as a valid
  // response of the `textDocument/codeAction` request. If the property is not
  // set the request can only return `Command` literals.
  // 
  // @since 3.8.0
  [[nodiscard]] ClientCodeActionLiteralOptions codeActionLiteralSupport();
  [[nodiscard]] bool has_codeActionLiteralSupport() const;
  [[nodiscard]] ClientCodeActionLiteralOptions add_codeActionLiteralSupport();

  // Whether code action supports the `isPreferred` property.
  // 
  // @since 3.15.0
  [[nodiscard]] bool isPreferredSupport();
  [[nodiscard]] bool has_isPreferredSupport() const;
  void set_isPreferredSupport(bool val);

  // Whether code action supports the `disabled` property.
  // 
  // @since 3.16.0
  [[nodiscard]] bool disabledSupport();
  [[nodiscard]] bool has_disabledSupport() const;
  void set_disabledSupport(bool val);

  // Whether code action supports the `data` property which is
  // preserved between a `textDocument/codeAction` and a
  // `codeAction/resolve` request.
  // 
  // @since 3.16.0
  [[nodiscard]] bool dataSupport();
  [[nodiscard]] bool has_dataSupport() const;
  void set_dataSupport(bool val);

  // Whether the client supports resolving additional code action
  // properties via a separate `codeAction/resolve` request.
  // 
  // @since 3.16.0
  [[nodiscard]] ClientCodeActionResolveOptions resolveSupport();
  [[nodiscard]] bool has_resolveSupport() const;
  [[nodiscard]] ClientCodeActionResolveOptions add_resolveSupport();

  // Whether the client honors the change annotations in
  // text edits and resource operations returned via the
  // `CodeAction#edit` property by for example presenting
  // the workspace edit in the user interface and asking
  // for confirmation.
  // 
  // @since 3.16.0
  [[nodiscard]] bool honorsChangeAnnotations();
  [[nodiscard]] bool has_honorsChangeAnnotations() const;
  void set_honorsChangeAnnotations(bool val);

  // Whether the client supports documentation for a class of
  // code actions.
  // 
  // @since 3.18.0
  // @proposed
  [[nodiscard]] bool documentationSupport();
  [[nodiscard]] bool has_documentationSupport() const;
  void set_documentationSupport(bool val);

  // Client supports the tag property on a code action. Clients
  // supporting tags have to handle unknown tags gracefully.
  // 
  // @since 3.18.0 - proposed
  [[nodiscard]] CodeActionTagOptions tagSupport();
  [[nodiscard]] bool has_tagSupport() const;
  [[nodiscard]] CodeActionTagOptions add_tagSupport();

  void init();
};

/**
 * Represents the signature of something callable. A signature
 * can have a label, like a function-name, a doc-comment, and
 * a set of parameters.
 */
struct SignatureInformation : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The label of this signature. Will be shown in
  // the UI.
  [[nodiscard]] std::string_view label();
  void set_label(std::string_view val);

  // The human-readable doc-comment of this signature. Will be shown
  // in the UI but can be omitted.
  struct Union1 : detail::UnionStructWrapper<std::string_view, MarkupContent> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_MarkupContent() const;
    [[nodiscard]] MarkupContent as_MarkupContent();

  };

  [[nodiscard]] SignatureInformation::Union1 documentation();
  [[nodiscard]] bool has_documentation() const;
  [[nodiscard]] SignatureInformation::Union1 add_documentation();

  // The parameters of this signature.
  [[nodiscard]] List<ParameterInformation> parameters();
  [[nodiscard]] bool has_parameters() const;
  [[nodiscard]] List<ParameterInformation> add_parameters();

  // The index of the active parameter.
  // 
  // If `null`, no parameter of the signature is active (for example a named
  // argument that does not match any declared parameters). This is only valid
  // if the client specifies the client capability
  // `textDocument.signatureHelp.noActiveParameterSupport === true`
  // 
  // If provided (or `null`), this is used in place of
  // `SignatureHelp.activeParameter`.
  // 
  // @since 3.16.0
  struct Union3 : detail::UnionStructWrapper<std::uint32_t> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

  };

  [[nodiscard]] SignatureInformation::Union3 activeParameter();
  [[nodiscard]] bool has_activeParameter() const;
  [[nodiscard]] SignatureInformation::Union3 add_activeParameter();

  void init();
};

/**
 * The publish diagnostic client capabilities.
 */
struct PublishDiagnosticsClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether the client interprets the version property of the
  // `textDocument/publishDiagnostics` notification's parameter.
  // 
  // @since 3.15.0
  [[nodiscard]] bool versionSupport();
  [[nodiscard]] bool has_versionSupport() const;
  void set_versionSupport(bool val);

  // Whether the clients accepts diagnostics with related information.
  [[nodiscard]] bool relatedInformation();
  [[nodiscard]] bool has_relatedInformation() const;
  void set_relatedInformation(bool val);

  // Client supports the tag property to provide meta data about a diagnostic.
  // Clients supporting tags have to handle unknown tags gracefully.
  // 
  // @since 3.15.0
  [[nodiscard]] ClientDiagnosticsTagOptions tagSupport();
  [[nodiscard]] bool has_tagSupport() const;
  [[nodiscard]] ClientDiagnosticsTagOptions add_tagSupport();

  // Client supports a codeDescription property
  // 
  // @since 3.16.0
  [[nodiscard]] bool codeDescriptionSupport();
  [[nodiscard]] bool has_codeDescriptionSupport() const;
  void set_codeDescriptionSupport(bool val);

  // Whether code action supports the `data` property which is
  // preserved between a `textDocument/publishDiagnostics` and
  // `textDocument/codeAction` request.
  // 
  // @since 3.16.0
  [[nodiscard]] bool dataSupport();
  [[nodiscard]] bool has_dataSupport() const;
  void set_dataSupport(bool val);

  void init();
};

/**
 * Client capabilities specific to diagnostic pull requests.
 * 
 * @since 3.17.0
 */
struct DiagnosticClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)`
  // return value for the corresponding server capability as well.
  [[nodiscard]] bool dynamicRegistration();
  [[nodiscard]] bool has_dynamicRegistration() const;
  void set_dynamicRegistration(bool val);

  // Whether the clients supports related documents for document diagnostic pulls.
  [[nodiscard]] bool relatedDocumentSupport();
  [[nodiscard]] bool has_relatedDocumentSupport() const;
  void set_relatedDocumentSupport(bool val);

  // Whether the clients accepts diagnostics with related information.
  [[nodiscard]] bool relatedInformation();
  [[nodiscard]] bool has_relatedInformation() const;
  void set_relatedInformation(bool val);

  // Client supports the tag property to provide meta data about a diagnostic.
  // Clients supporting tags have to handle unknown tags gracefully.
  // 
  // @since 3.15.0
  [[nodiscard]] ClientDiagnosticsTagOptions tagSupport();
  [[nodiscard]] bool has_tagSupport() const;
  [[nodiscard]] ClientDiagnosticsTagOptions add_tagSupport();

  // Client supports a codeDescription property
  // 
  // @since 3.16.0
  [[nodiscard]] bool codeDescriptionSupport();
  [[nodiscard]] bool has_codeDescriptionSupport() const;
  void set_codeDescriptionSupport(bool val);

  // Whether code action supports the `data` property which is
  // preserved between a `textDocument/publishDiagnostics` and
  // `textDocument/codeAction` request.
  // 
  // @since 3.16.0
  [[nodiscard]] bool dataSupport();
  [[nodiscard]] bool has_dataSupport() const;
  void set_dataSupport(bool val);

  void init();
};

/**
 * A notebook document.
 * 
 * @since 3.17.0
 */
struct NotebookDocument : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The notebook document's uri.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  // The type of the notebook.
  [[nodiscard]] std::string_view notebookType();
  void set_notebookType(std::string_view val);

  // The version number of this document (it will increase after each
  // change, including undo/redo).
  [[nodiscard]] std::int32_t version();
  void set_version(std::int32_t val);

  // Additional metadata stored with the notebook
  // document.
  // 
  // Note: should always be an object literal (e.g. LSPObject)
  [[nodiscard]] LSPObject metadata();
  [[nodiscard]] bool has_metadata() const;
  [[nodiscard]] LSPObject add_metadata();

  // The cells of a notebook.
  [[nodiscard]] List<NotebookCell> cells();

  void init();
};

/**
 * A change describing how to move a `NotebookCell`
 * array from state S to S'.
 * 
 * @since 3.17.0
 */
struct NotebookCellArrayChange : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The start oftest of the cell that changed.
  [[nodiscard]] std::uint32_t start();
  void set_start(std::uint32_t val);

  // The deleted cells
  [[nodiscard]] std::uint32_t deleteCount();
  void set_deleteCount(std::uint32_t val);

  // The new cells, if any
  [[nodiscard]] List<NotebookCell> cells();
  [[nodiscard]] bool has_cells() const;
  [[nodiscard]] List<NotebookCell> add_cells();

  void init();
};

/**
 * Provider options for a {@link CodeActionRequest}.
 */
struct CodeActionOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // CodeActionKinds that this server may return.
  // 
  // The list of kinds may be generic, such as `CodeActionKind.Refactor`, or the server
  // may list out every specific kind they provide.
  [[nodiscard]] List<CodeActionKind> codeActionKinds();
  [[nodiscard]] bool has_codeActionKinds() const;
  [[nodiscard]] List<CodeActionKind> add_codeActionKinds();

  // Static documentation for a class of code actions.
  // 
  // Documentation from the provider should be shown in the code actions menu if either:
  // 
  // - Code actions of `kind` are requested by the editor. In this case, the editor will show the documentation that
  //   most closely matches the requested code action kind. For example, if a provider has documentation for
  //   both `Refactor` and `RefactorExtract`, when the user requests code actions for `RefactorExtract`,
  //   the editor will use the documentation for `RefactorExtract` instead of the documentation for `Refactor`.
  // 
  // - Any code actions of `kind` are returned by the provider.
  // 
  // At most one documentation entry should be shown per provider.
  // 
  // @since 3.18.0
  // @proposed
  [[nodiscard]] List<CodeActionKindDocumentation> documentation();
  [[nodiscard]] bool has_documentation() const;
  [[nodiscard]] List<CodeActionKindDocumentation> add_documentation();

  // The server provides support to resolve additional
  // information for a code action.
  // 
  // @since 3.16.0
  [[nodiscard]] bool resolveProvider();
  [[nodiscard]] bool has_resolveProvider() const;
  void set_resolveProvider(bool val);
  [[nodiscard]] bool workDoneProgress();
  [[nodiscard]] bool has_workDoneProgress() const;
  void set_workDoneProgress(bool val);

  void init();
};

struct FileSystemWatcher : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The glob pattern to watch. See {@link GlobPattern glob pattern} for more detail.
  // 
  // @since 3.17.0 support for relative patterns.
  struct Union0 : detail::UnionStructWrapper<Pattern, RelativePattern> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] Pattern as_string();
    void set_string(Pattern val);

    [[nodiscard]] bool holds_RelativePattern() const;
    [[nodiscard]] RelativePattern as_RelativePattern();

  };

  [[nodiscard]] FileSystemWatcher::Union0 globPattern();

  // The kind of events of interest. If omitted it defaults
  // to WatchKind.Create | WatchKind.Change | WatchKind.Delete
  // which is 7.
  [[nodiscard]] WatchKind kind();
  [[nodiscard]] bool has_kind() const;
  void set_kind(WatchKind val);

  void init();
};

/**
 * A document filter where `language` is required field.
 * 
 * @since 3.18.0
 */
struct TextDocumentFilterLanguage : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A language id, like `typescript`.
  [[nodiscard]] std::string_view language();
  void set_language(std::string_view val);

  // A Uri {@link Uri.scheme scheme}, like `file` or `untitled`.
  [[nodiscard]] std::string_view scheme();
  [[nodiscard]] bool has_scheme() const;
  void set_scheme(std::string_view val);

  // A glob pattern, like **​/\*.{ts,js}. See TextDocumentFilter for examples.
  // 
  // @since 3.18.0 - support for relative patterns. Whether clients support
  // relative patterns depends on the client capability
  // `textDocuments.filters.relativePatternSupport`.
  struct Union2 : detail::UnionStructWrapper<Pattern, RelativePattern> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] Pattern as_string();
    void set_string(Pattern val);

    [[nodiscard]] bool holds_RelativePattern() const;
    [[nodiscard]] RelativePattern as_RelativePattern();

  };

  [[nodiscard]] TextDocumentFilterLanguage::Union2 pattern();
  [[nodiscard]] bool has_pattern() const;
  [[nodiscard]] TextDocumentFilterLanguage::Union2 add_pattern();

  void init();
};

/**
 * A document filter where `scheme` is required field.
 * 
 * @since 3.18.0
 */
struct TextDocumentFilterScheme : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A language id, like `typescript`.
  [[nodiscard]] std::string_view language();
  [[nodiscard]] bool has_language() const;
  void set_language(std::string_view val);

  // A Uri {@link Uri.scheme scheme}, like `file` or `untitled`.
  [[nodiscard]] std::string_view scheme();
  void set_scheme(std::string_view val);

  // A glob pattern, like **​/\*.{ts,js}. See TextDocumentFilter for examples.
  // 
  // @since 3.18.0 - support for relative patterns. Whether clients support
  // relative patterns depends on the client capability
  // `textDocuments.filters.relativePatternSupport`.
  struct Union2 : detail::UnionStructWrapper<Pattern, RelativePattern> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] Pattern as_string();
    void set_string(Pattern val);

    [[nodiscard]] bool holds_RelativePattern() const;
    [[nodiscard]] RelativePattern as_RelativePattern();

  };

  [[nodiscard]] TextDocumentFilterScheme::Union2 pattern();
  [[nodiscard]] bool has_pattern() const;
  [[nodiscard]] TextDocumentFilterScheme::Union2 add_pattern();

  void init();
};

/**
 * A document filter where `pattern` is required field.
 * 
 * @since 3.18.0
 */
struct TextDocumentFilterPattern : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A language id, like `typescript`.
  [[nodiscard]] std::string_view language();
  [[nodiscard]] bool has_language() const;
  void set_language(std::string_view val);

  // A Uri {@link Uri.scheme scheme}, like `file` or `untitled`.
  [[nodiscard]] std::string_view scheme();
  [[nodiscard]] bool has_scheme() const;
  void set_scheme(std::string_view val);

  // A glob pattern, like **​/\*.{ts,js}. See TextDocumentFilter for examples.
  // 
  // @since 3.18.0 - support for relative patterns. Whether clients support
  // relative patterns depends on the client capability
  // `textDocuments.filters.relativePatternSupport`.
  struct Union2 : detail::UnionStructWrapper<Pattern, RelativePattern> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] Pattern as_string();
    void set_string(Pattern val);

    [[nodiscard]] bool holds_RelativePattern() const;
    [[nodiscard]] RelativePattern as_RelativePattern();

  };

  [[nodiscard]] TextDocumentFilterPattern::Union2 pattern();

  void init();
};

/**
 * A notebook document filter where `notebookType` is required field.
 * 
 * @since 3.18.0
 */
struct NotebookDocumentFilterNotebookType : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The type of the enclosing notebook.
  [[nodiscard]] std::string_view notebookType();
  void set_notebookType(std::string_view val);

  // A Uri {@link Uri.scheme scheme}, like `file` or `untitled`.
  [[nodiscard]] std::string_view scheme();
  [[nodiscard]] bool has_scheme() const;
  void set_scheme(std::string_view val);

  // A glob pattern.
  struct Union2 : detail::UnionStructWrapper<Pattern, RelativePattern> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] Pattern as_string();
    void set_string(Pattern val);

    [[nodiscard]] bool holds_RelativePattern() const;
    [[nodiscard]] RelativePattern as_RelativePattern();

  };

  [[nodiscard]] NotebookDocumentFilterNotebookType::Union2 pattern();
  [[nodiscard]] bool has_pattern() const;
  [[nodiscard]] NotebookDocumentFilterNotebookType::Union2 add_pattern();

  void init();
};

/**
 * A notebook document filter where `scheme` is required field.
 * 
 * @since 3.18.0
 */
struct NotebookDocumentFilterScheme : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The type of the enclosing notebook.
  [[nodiscard]] std::string_view notebookType();
  [[nodiscard]] bool has_notebookType() const;
  void set_notebookType(std::string_view val);

  // A Uri {@link Uri.scheme scheme}, like `file` or `untitled`.
  [[nodiscard]] std::string_view scheme();
  void set_scheme(std::string_view val);

  // A glob pattern.
  struct Union2 : detail::UnionStructWrapper<Pattern, RelativePattern> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] Pattern as_string();
    void set_string(Pattern val);

    [[nodiscard]] bool holds_RelativePattern() const;
    [[nodiscard]] RelativePattern as_RelativePattern();

  };

  [[nodiscard]] NotebookDocumentFilterScheme::Union2 pattern();
  [[nodiscard]] bool has_pattern() const;
  [[nodiscard]] NotebookDocumentFilterScheme::Union2 add_pattern();

  void init();
};

/**
 * A notebook document filter where `pattern` is required field.
 * 
 * @since 3.18.0
 */
struct NotebookDocumentFilterPattern : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The type of the enclosing notebook.
  [[nodiscard]] std::string_view notebookType();
  [[nodiscard]] bool has_notebookType() const;
  void set_notebookType(std::string_view val);

  // A Uri {@link Uri.scheme scheme}, like `file` or `untitled`.
  [[nodiscard]] std::string_view scheme();
  [[nodiscard]] bool has_scheme() const;
  void set_scheme(std::string_view val);

  // A glob pattern.
  struct Union2 : detail::UnionStructWrapper<Pattern, RelativePattern> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] Pattern as_string();
    void set_string(Pattern val);

    [[nodiscard]] bool holds_RelativePattern() const;
    [[nodiscard]] RelativePattern as_RelativePattern();

  };

  [[nodiscard]] NotebookDocumentFilterPattern::Union2 pattern();

  void init();
};

/**
 * The definition of a symbol represented as one or many {@link Location locations}.
 * For most programming languages there is only one location at which a symbol is
 * defined.
 * 
 * Servers should prefer returning `DefinitionLink` over `Definition` if supported
 * by the client.
 */
struct Definition : detail::StructWrapper {
  using StructWrapper::StructWrapper;

};

/**
 * The declaration of a symbol representation as one or many {@link Location locations}.
 */
struct Declaration : detail::StructWrapper {
  using StructWrapper::StructWrapper;

};

/**
 * Represents information about programming constructs like variables, classes,
 * interfaces etc.
 */
struct SymbolInformation : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Indicates if this symbol is deprecated.
  // 
  // @deprecated Use tags instead
  [[nodiscard]] bool deprecated();
  [[nodiscard]] bool has_deprecated() const;
  void set_deprecated(bool val);

  // The location of this symbol. The location's range is used by a tool
  // to reveal the location in the editor. If the symbol is selected in the
  // tool the range's start information is used to position the cursor. So
  // the range usually spans more than the actual symbol's name and does
  // normally include things like visibility modifiers.
  // 
  // The range doesn't have to denote a node range in the sense of an abstract
  // syntax tree. It can therefore not be used to re-construct a hierarchy of
  // the symbols.
  [[nodiscard]] Location location();

  // The name of this symbol.
  [[nodiscard]] std::string_view name();
  void set_name(std::string_view val);

  // The kind of this symbol.
  [[nodiscard]] SymbolKind kind();
  void set_kind(SymbolKind val);

  // Tags for this symbol.
  // 
  // @since 3.16.0
  [[nodiscard]] List<SymbolTag> tags();
  [[nodiscard]] bool has_tags() const;
  [[nodiscard]] List<SymbolTag> add_tags();

  // The name of the symbol containing this symbol. This information is for
  // user interface purposes (e.g. to render a qualifier in the user interface
  // if necessary). It can't be used to re-infer a hierarchy for the document
  // symbols.
  [[nodiscard]] std::string_view containerName();
  [[nodiscard]] bool has_containerName() const;
  void set_containerName(std::string_view val);

  void init();
};

/**
 * A special workspace symbol that supports locations without a range.
 * 
 * See also SymbolInformation.
 * 
 * @since 3.17.0
 */
struct WorkspaceSymbol : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The location of the symbol. Whether a server is allowed to
  // return a location without a range depends on the client
  // capability `workspace.symbol.resolveSupport`.
  // 
  // See SymbolInformation#location for more details.
  struct Union0 : detail::UnionStructWrapper<Location, LocationUriOnly> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_Location() const;
    [[nodiscard]] Location as_Location();

    [[nodiscard]] bool holds_LocationUriOnly() const;
    [[nodiscard]] LocationUriOnly as_LocationUriOnly();

  };

  [[nodiscard]] WorkspaceSymbol::Union0 location();

  // A data entry field that is preserved on a workspace symbol between a
  // workspace symbol request and a workspace symbol resolve request.
  struct Union1 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] WorkspaceSymbol::Union1 data();
  [[nodiscard]] bool has_data() const;
  [[nodiscard]] WorkspaceSymbol::Union1 add_data();

  // The name of this symbol.
  [[nodiscard]] std::string_view name();
  void set_name(std::string_view val);

  // The kind of this symbol.
  [[nodiscard]] SymbolKind kind();
  void set_kind(SymbolKind val);

  // Tags for this symbol.
  // 
  // @since 3.16.0
  [[nodiscard]] List<SymbolTag> tags();
  [[nodiscard]] bool has_tags() const;
  [[nodiscard]] List<SymbolTag> add_tags();

  // The name of the symbol containing this symbol. This information is for
  // user interface purposes (e.g. to render a qualifier in the user interface
  // if necessary). It can't be used to re-infer a hierarchy for the document
  // symbols.
  [[nodiscard]] std::string_view containerName();
  [[nodiscard]] bool has_containerName() const;
  void set_containerName(std::string_view val);

  void init();
};

/**
 * An inlay hint label part allows for interactive and composite labels
 * of inlay hints.
 * 
 * @since 3.17.0
 */
struct InlayHintLabelPart : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The value of this label part.
  [[nodiscard]] std::string_view value();
  void set_value(std::string_view val);

  // The tooltip text when you hover over this label part. Depending on
  // the client capability `inlayHint.resolveSupport` clients might resolve
  // this property late using the resolve request.
  struct Union1 : detail::UnionStructWrapper<std::string_view, MarkupContent> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_MarkupContent() const;
    [[nodiscard]] MarkupContent as_MarkupContent();

  };

  [[nodiscard]] InlayHintLabelPart::Union1 tooltip();
  [[nodiscard]] bool has_tooltip() const;
  [[nodiscard]] InlayHintLabelPart::Union1 add_tooltip();

  // An optional source code location that represents this
  // label part.
  // 
  // The editor will use this location for the hover and for code navigation
  // features: This part will become a clickable link that resolves to the
  // definition of the symbol at the given location (not necessarily the
  // location itself), it shows the hover that shows at the given location,
  // and it shows a context menu with further code navigation commands.
  // 
  // Depending on the client capability `inlayHint.resolveSupport` clients
  // might resolve this property late using the resolve request.
  [[nodiscard]] Location location();
  [[nodiscard]] bool has_location() const;
  [[nodiscard]] Location add_location();

  // An optional command for this label part.
  // 
  // Depending on the client capability `inlayHint.resolveSupport` clients
  // might resolve this property late using the resolve request.
  [[nodiscard]] Command command();
  [[nodiscard]] bool has_command() const;
  [[nodiscard]] Command add_command();

  void init();
};

/**
 * Represents a related message and source code location for a diagnostic. This should be
 * used to point to code locations that cause or related to a diagnostics, e.g when duplicating
 * a symbol in a scope.
 */
struct DiagnosticRelatedInformation : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The location of this related diagnostic information.
  [[nodiscard]] Location location();

  // The message of this related diagnostic information.
  [[nodiscard]] std::string_view message();
  void set_message(std::string_view val);

  void init();
};

/**
 * The parameter of a `callHierarchy/incomingCalls` request.
 * 
 * @since 3.16.0
 */
struct CallHierarchyIncomingCallsParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] CallHierarchyItem item();

  // An optional token that a server can use to report work done progress.
  struct Union1 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] CallHierarchyIncomingCallsParams::Union1 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] CallHierarchyIncomingCallsParams::Union1 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] CallHierarchyIncomingCallsParams::Union2 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] CallHierarchyIncomingCallsParams::Union2 add_partialResultToken();

  void init();
};

/**
 * Represents an incoming call, e.g. a caller of a method or constructor.
 * 
 * @since 3.16.0
 */
struct CallHierarchyIncomingCall : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The item that makes the call.
  [[nodiscard]] CallHierarchyItem from();

  // The ranges at which the calls appear. This is relative to the caller
  // denoted by {@link CallHierarchyIncomingCall.from `this.from`}.
  [[nodiscard]] List<Range> fromRanges();

  void init();
};

/**
 * The parameter of a `callHierarchy/outgoingCalls` request.
 * 
 * @since 3.16.0
 */
struct CallHierarchyOutgoingCallsParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] CallHierarchyItem item();

  // An optional token that a server can use to report work done progress.
  struct Union1 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] CallHierarchyOutgoingCallsParams::Union1 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] CallHierarchyOutgoingCallsParams::Union1 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] CallHierarchyOutgoingCallsParams::Union2 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] CallHierarchyOutgoingCallsParams::Union2 add_partialResultToken();

  void init();
};

/**
 * Represents an outgoing call, e.g. calling a getter from a method or a method from a constructor etc.
 * 
 * @since 3.16.0
 */
struct CallHierarchyOutgoingCall : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The item that is called.
  [[nodiscard]] CallHierarchyItem to();

  // The range at which this item is called. This is the range relative to the caller, e.g the item
  // passed to {@link CallHierarchyItemProvider.provideCallHierarchyOutgoingCalls `provideCallHierarchyOutgoingCalls`}
  // and not {@link CallHierarchyOutgoingCall.to `this.to`}.
  [[nodiscard]] List<Range> fromRanges();

  void init();
};

/**
 * The parameter of a `typeHierarchy/supertypes` request.
 * 
 * @since 3.17.0
 */
struct TypeHierarchySupertypesParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] TypeHierarchyItem item();

  // An optional token that a server can use to report work done progress.
  struct Union1 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] TypeHierarchySupertypesParams::Union1 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] TypeHierarchySupertypesParams::Union1 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] TypeHierarchySupertypesParams::Union2 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] TypeHierarchySupertypesParams::Union2 add_partialResultToken();

  void init();
};

/**
 * The parameter of a `typeHierarchy/subtypes` request.
 * 
 * @since 3.17.0
 */
struct TypeHierarchySubtypesParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] TypeHierarchyItem item();

  // An optional token that a server can use to report work done progress.
  struct Union1 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] TypeHierarchySubtypesParams::Union1 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] TypeHierarchySubtypesParams::Union1 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] TypeHierarchySubtypesParams::Union2 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] TypeHierarchySubtypesParams::Union2 add_partialResultToken();

  void init();
};

/**
 * Represents a collection of {@link InlineCompletionItem inline completion items} to be presented in the editor.
 * 
 * @since 3.18.0
 * @proposed
 */
struct InlineCompletionList : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The inline completion items
  [[nodiscard]] List<InlineCompletionItem> items();

  void init();
};

struct ColorPresentation : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The label of this color presentation. It will be shown on the color
  // picker header. By default this is also the text that is inserted when selecting
  // this color presentation.
  [[nodiscard]] std::string_view label();
  void set_label(std::string_view val);

  // An {@link TextEdit edit} which is applied to a document when selecting
  // this presentation for the color.  When `falsy` the {@link ColorPresentation.label label}
  // is used.
  [[nodiscard]] TextEdit textEdit();
  [[nodiscard]] bool has_textEdit() const;
  [[nodiscard]] TextEdit add_textEdit();

  // An optional array of additional {@link TextEdit text edits} that are applied when
  // selecting this color presentation. Edits must not overlap with the main {@link ColorPresentation.textEdit edit} nor with themselves.
  [[nodiscard]] List<TextEdit> additionalTextEdits();
  [[nodiscard]] bool has_additionalTextEdits() const;
  [[nodiscard]] List<TextEdit> add_additionalTextEdits();

  void init();
};

/**
 * A special text edit with an additional change annotation.
 * 
 * @since 3.16.0.
 */
struct AnnotatedTextEdit : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The actual identifier of the change annotation
  [[nodiscard]] ChangeAnnotationIdentifier annotationId();
  void set_annotationId(ChangeAnnotationIdentifier val);

  // The range of the text document to be manipulated. To insert
  // text into a document create a range where start === end.
  [[nodiscard]] Range range();

  // The string to be inserted. For delete operations use an
  // empty string.
  [[nodiscard]] std::string_view newText();
  void set_newText(std::string_view val);

  void init();
};

/**
 * Information about where a symbol is defined.
 * 
 * Provides additional metadata over normal {@link Location location} definitions, including the range of
 * the defining symbol
 */
using DefinitionLink = LocationLink;

/**
 * Information about where a symbol is declared.
 * 
 * Provides additional metadata over normal {@link Location location} declarations, including the range of
 * the declaring symbol.
 * 
 * Servers should prefer returning `DeclarationLink` over `Declaration` if supported
 * by the client.
 */
using DeclarationLink = LocationLink;

/**
 * A parameter literal used in inline value requests.
 * 
 * @since 3.17.0
 */
struct InlineValueParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The document range for which inline values should be computed.
  [[nodiscard]] Range range();

  // Additional information about the context in which inline values were
  // requested.
  [[nodiscard]] InlineValueContext context();

  // An optional token that a server can use to report work done progress.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] InlineValueParams::Union3 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] InlineValueParams::Union3 add_workDoneToken();

  void init();
};

/**
 * Inline value information can be provided by different means:
 * - directly as a text value (class InlineValueText).
 * - as a name to use for a variable lookup (class InlineValueVariableLookup)
 * - as an evaluatable expression (class InlineValueEvaluatableExpression)
 * The InlineValue types combines all inline value types into one type.
 * 
 * @since 3.17.0
 */
struct InlineValue : detail::StructWrapper {
  using StructWrapper::StructWrapper;

};

/**
 * A completion item represents a text snippet that is
 * proposed to complete text that is being typed.
 */
struct CompletionItem : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The label of this completion item.
  // 
  // The label property is also by default the text that
  // is inserted when selecting this completion.
  // 
  // If label details are provided the label itself should
  // be an unqualified name of the completion item.
  [[nodiscard]] std::string_view label();
  void set_label(std::string_view val);

  // Additional details for the label
  // 
  // @since 3.17.0
  [[nodiscard]] CompletionItemLabelDetails labelDetails();
  [[nodiscard]] bool has_labelDetails() const;
  [[nodiscard]] CompletionItemLabelDetails add_labelDetails();

  // The kind of this completion item. Based of the kind
  // an icon is chosen by the editor.
  [[nodiscard]] CompletionItemKind kind();
  [[nodiscard]] bool has_kind() const;
  void set_kind(CompletionItemKind val);

  // Tags for this completion item.
  // 
  // @since 3.15.0
  [[nodiscard]] List<CompletionItemTag> tags();
  [[nodiscard]] bool has_tags() const;
  [[nodiscard]] List<CompletionItemTag> add_tags();

  // A human-readable string with additional information
  // about this item, like type or symbol information.
  [[nodiscard]] std::string_view detail();
  [[nodiscard]] bool has_detail() const;
  void set_detail(std::string_view val);

  // A human-readable string that represents a doc-comment.
  struct Union5 : detail::UnionStructWrapper<std::string_view, MarkupContent> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_MarkupContent() const;
    [[nodiscard]] MarkupContent as_MarkupContent();

  };

  [[nodiscard]] CompletionItem::Union5 documentation();
  [[nodiscard]] bool has_documentation() const;
  [[nodiscard]] CompletionItem::Union5 add_documentation();

  // Indicates if this item is deprecated.
  // @deprecated Use `tags` instead.
  [[nodiscard]] bool deprecated();
  [[nodiscard]] bool has_deprecated() const;
  void set_deprecated(bool val);

  // Select this item when showing.
  // 
  // *Note* that only one completion item can be selected and that the
  // tool / client decides which item that is. The rule is that the *first*
  // item of those that match best is selected.
  [[nodiscard]] bool preselect();
  [[nodiscard]] bool has_preselect() const;
  void set_preselect(bool val);

  // A string that should be used when comparing this item
  // with other items. When `falsy` the {@link CompletionItem.label label}
  // is used.
  [[nodiscard]] std::string_view sortText();
  [[nodiscard]] bool has_sortText() const;
  void set_sortText(std::string_view val);

  // A string that should be used when filtering a set of
  // completion items. When `falsy` the {@link CompletionItem.label label}
  // is used.
  [[nodiscard]] std::string_view filterText();
  [[nodiscard]] bool has_filterText() const;
  void set_filterText(std::string_view val);

  // A string that should be inserted into a document when selecting
  // this completion. When `falsy` the {@link CompletionItem.label label}
  // is used.
  // 
  // The `insertText` is subject to interpretation by the client side.
  // Some tools might not take the string literally. For example
  // VS Code when code complete is requested in this example
  // `con<cursor position>` and a completion item with an `insertText` of
  // `console` is provided it will only insert `sole`. Therefore it is
  // recommended to use `textEdit` instead since it avoids additional client
  // side interpretation.
  [[nodiscard]] std::string_view insertText();
  [[nodiscard]] bool has_insertText() const;
  void set_insertText(std::string_view val);

  // The format of the insert text. The format applies to both the
  // `insertText` property and the `newText` property of a provided
  // `textEdit`. If omitted defaults to `InsertTextFormat.PlainText`.
  // 
  // Please note that the insertTextFormat doesn't apply to
  // `additionalTextEdits`.
  [[nodiscard]] InsertTextFormat insertTextFormat();
  [[nodiscard]] bool has_insertTextFormat() const;
  void set_insertTextFormat(InsertTextFormat val);

  // How whitespace and indentation is handled during completion
  // item insertion. If not provided the clients default value depends on
  // the `textDocument.completion.insertTextMode` client capability.
  // 
  // @since 3.16.0
  [[nodiscard]] InsertTextMode insertTextMode();
  [[nodiscard]] bool has_insertTextMode() const;
  void set_insertTextMode(InsertTextMode val);

  // An {@link TextEdit edit} which is applied to a document when selecting
  // this completion. When an edit is provided the value of
  // {@link CompletionItem.insertText insertText} is ignored.
  // 
  // Most editors support two different operations when accepting a completion
  // item. One is to insert a completion text and the other is to replace an
  // existing text with a completion text. Since this can usually not be
  // predetermined by a server it can report both ranges. Clients need to
  // signal support for `InsertReplaceEdits` via the
  // `textDocument.completion.insertReplaceSupport` client capability
  // property.
  // 
  // *Note 1:* The text edit's range as well as both ranges from an insert
  // replace edit must be a [single line] and they must contain the position
  // at which completion has been requested.
  // *Note 2:* If an `InsertReplaceEdit` is returned the edit's insert range
  // must be a prefix of the edit's replace range, that means it must be
  // contained and starting at the same position.
  // 
  // @since 3.16.0 additional type `InsertReplaceEdit`
  struct Union13 : detail::UnionStructWrapper<TextEdit, InsertReplaceEdit> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_TextEdit() const;
    [[nodiscard]] TextEdit as_TextEdit();

    [[nodiscard]] bool holds_InsertReplaceEdit() const;
    [[nodiscard]] InsertReplaceEdit as_InsertReplaceEdit();

  };

  [[nodiscard]] CompletionItem::Union13 textEdit();
  [[nodiscard]] bool has_textEdit() const;
  [[nodiscard]] CompletionItem::Union13 add_textEdit();

  // The edit text used if the completion item is part of a CompletionList and
  // CompletionList defines an item default for the text edit range.
  // 
  // Clients will only honor this property if they opt into completion list
  // item defaults using the capability `completionList.itemDefaults`.
  // 
  // If not provided and a list's default range is provided the label
  // property is used as a text.
  // 
  // @since 3.17.0
  [[nodiscard]] std::string_view textEditText();
  [[nodiscard]] bool has_textEditText() const;
  void set_textEditText(std::string_view val);

  // An optional array of additional {@link TextEdit text edits} that are applied when
  // selecting this completion. Edits must not overlap (including the same insert position)
  // with the main {@link CompletionItem.textEdit edit} nor with themselves.
  // 
  // Additional text edits should be used to change text unrelated to the current cursor position
  // (for example adding an import statement at the top of the file if the completion item will
  // insert an unqualified type).
  [[nodiscard]] List<TextEdit> additionalTextEdits();
  [[nodiscard]] bool has_additionalTextEdits() const;
  [[nodiscard]] List<TextEdit> add_additionalTextEdits();

  // An optional set of characters that when pressed while this completion is active will accept it first and
  // then type that character. *Note* that all commit characters should have `length=1` and that superfluous
  // characters will be ignored.
  [[nodiscard]] List<std::string_view> commitCharacters();
  [[nodiscard]] bool has_commitCharacters() const;
  [[nodiscard]] List<std::string_view> add_commitCharacters();

  // An optional {@link Command command} that is executed *after* inserting this completion. *Note* that
  // additional modifications to the current document should be described with the
  // {@link CompletionItem.additionalTextEdits additionalTextEdits}-property.
  [[nodiscard]] Command command();
  [[nodiscard]] bool has_command() const;
  [[nodiscard]] Command add_command();

  // A data entry field that is preserved on a completion item between a
  // {@link CompletionRequest} and a {@link CompletionResolveRequest}.
  struct Union18 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] CompletionItem::Union18 data();
  [[nodiscard]] bool has_data() const;
  [[nodiscard]] CompletionItem::Union18 add_data();

  void init();
};

struct PrepareRenameResult : detail::StructWrapper {
  using StructWrapper::StructWrapper;

};

/**
 * Provides information about the context in which an inline completion was requested.
 * 
 * @since 3.18.0
 * @proposed
 */
struct InlineCompletionContext : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Describes how the inline completion was triggered.
  [[nodiscard]] InlineCompletionTriggerKind triggerKind();
  void set_triggerKind(InlineCompletionTriggerKind val);

  // Provides information about the currently selected item in the autocomplete widget if it is visible.
  [[nodiscard]] SelectedCompletionInfo selectedCompletionInfo();
  [[nodiscard]] bool has_selectedCompletionInfo() const;
  [[nodiscard]] SelectedCompletionInfo add_selectedCompletionInfo();

  void init();
};

/**
 * An event describing a change to a text document. If only a text is provided
 * it is considered to be the full content of the document.
 */
struct TextDocumentContentChangeEvent : detail::StructWrapper {
  using StructWrapper::StructWrapper;

};

/**
 * In many cases the items of an actual completion result share the same
 * value for properties like `commitCharacters` or the range of a text
 * edit. A completion list can therefore define item defaults which will
 * be used if a completion item itself doesn't specify the value.
 * 
 * If a completion list specifies a default value and a completion item
 * also specifies a corresponding value, the rules for combining these are
 * defined by `applyKinds` (if the client supports it), defaulting to
 * ApplyKind.Replace.
 * 
 * Servers are only allowed to return default values if the client
 * signals support for this via the `completionList.itemDefaults`
 * capability.
 * 
 * @since 3.17.0
 */
struct CompletionItemDefaults : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A default commit character set.
  // 
  // @since 3.17.0
  [[nodiscard]] List<std::string_view> commitCharacters();
  [[nodiscard]] bool has_commitCharacters() const;
  [[nodiscard]] List<std::string_view> add_commitCharacters();

  // A default edit range.
  // 
  // @since 3.17.0
  struct Union1 : detail::UnionStructWrapper<Range, EditRangeWithInsertReplace> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_Range() const;
    [[nodiscard]] Range as_Range();

    [[nodiscard]] bool holds_EditRangeWithInsertReplace() const;
    [[nodiscard]] EditRangeWithInsertReplace as_EditRangeWithInsertReplace();

  };

  [[nodiscard]] CompletionItemDefaults::Union1 editRange();
  [[nodiscard]] bool has_editRange() const;
  [[nodiscard]] CompletionItemDefaults::Union1 add_editRange();

  // A default insert text format.
  // 
  // @since 3.17.0
  [[nodiscard]] InsertTextFormat insertTextFormat();
  [[nodiscard]] bool has_insertTextFormat() const;
  void set_insertTextFormat(InsertTextFormat val);

  // A default insert text mode.
  // 
  // @since 3.17.0
  [[nodiscard]] InsertTextMode insertTextMode();
  [[nodiscard]] bool has_insertTextMode() const;
  void set_insertTextMode(InsertTextMode val);

  // A default data value.
  // 
  // @since 3.17.0
  struct Union4 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] CompletionItemDefaults::Union4 data();
  [[nodiscard]] bool has_data() const;
  [[nodiscard]] CompletionItemDefaults::Union4 add_data();

  void init();
};

/**
 * The options to register for file operations.
 * 
 * @since 3.16.0
 */
struct FileOperationRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The actual filters.
  [[nodiscard]] List<FileOperationFilter> filters();

  void init();
};

/**
 * Signature help represents the signature of something
 * callable. There can be multiple signature but only one
 * active and only one active parameter.
 */
struct SignatureHelp : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // One or more signatures.
  [[nodiscard]] List<SignatureInformation> signatures();

  // The active signature. If omitted or the value lies outside the
  // range of `signatures` the value defaults to zero or is ignored if
  // the `SignatureHelp` has no signatures.
  // 
  // Whenever possible implementors should make an active decision about
  // the active signature and shouldn't rely on a default value.
  // 
  // In future version of the protocol this property might become
  // mandatory to better express this.
  [[nodiscard]] std::uint32_t activeSignature();
  [[nodiscard]] bool has_activeSignature() const;
  void set_activeSignature(std::uint32_t val);

  // The active parameter of the active signature.
  // 
  // If `null`, no parameter of the signature is active (for example a named
  // argument that does not match any declared parameters). This is only valid
  // if the client specifies the client capability
  // `textDocument.signatureHelp.noActiveParameterSupport === true`
  // 
  // If omitted or the value lies outside the range of
  // `signatures[activeSignature].parameters` defaults to 0 if the active
  // signature has parameters.
  // 
  // If the active signature has no parameters it is ignored.
  // 
  // In future version of the protocol this property might become
  // mandatory (but still nullable) to better express the active parameter if
  // the active signature does have any.
  struct Union2 : detail::UnionStructWrapper<std::uint32_t> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

  };

  [[nodiscard]] SignatureHelp::Union2 activeParameter();
  [[nodiscard]] bool has_activeParameter() const;
  [[nodiscard]] SignatureHelp::Union2 add_activeParameter();

  void init();
};

/**
 * Text document specific client capabilities.
 */
struct TextDocumentClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Defines which synchronization capabilities the client supports.
  [[nodiscard]] TextDocumentSyncClientCapabilities synchronization();
  [[nodiscard]] bool has_synchronization() const;
  [[nodiscard]] TextDocumentSyncClientCapabilities add_synchronization();

  // Defines which filters the client supports.
  // 
  // @since 3.18.0
  [[nodiscard]] TextDocumentFilterClientCapabilities filters();
  [[nodiscard]] bool has_filters() const;
  [[nodiscard]] TextDocumentFilterClientCapabilities add_filters();

  // Capabilities specific to the `textDocument/completion` request.
  [[nodiscard]] CompletionClientCapabilities completion();
  [[nodiscard]] bool has_completion() const;
  [[nodiscard]] CompletionClientCapabilities add_completion();

  // Capabilities specific to the `textDocument/hover` request.
  [[nodiscard]] HoverClientCapabilities hover();
  [[nodiscard]] bool has_hover() const;
  [[nodiscard]] HoverClientCapabilities add_hover();

  // Capabilities specific to the `textDocument/signatureHelp` request.
  [[nodiscard]] SignatureHelpClientCapabilities signatureHelp();
  [[nodiscard]] bool has_signatureHelp() const;
  [[nodiscard]] SignatureHelpClientCapabilities add_signatureHelp();

  // Capabilities specific to the `textDocument/declaration` request.
  // 
  // @since 3.14.0
  [[nodiscard]] DeclarationClientCapabilities declaration();
  [[nodiscard]] bool has_declaration() const;
  [[nodiscard]] DeclarationClientCapabilities add_declaration();

  // Capabilities specific to the `textDocument/definition` request.
  [[nodiscard]] DefinitionClientCapabilities definition();
  [[nodiscard]] bool has_definition() const;
  [[nodiscard]] DefinitionClientCapabilities add_definition();

  // Capabilities specific to the `textDocument/typeDefinition` request.
  // 
  // @since 3.6.0
  [[nodiscard]] TypeDefinitionClientCapabilities typeDefinition();
  [[nodiscard]] bool has_typeDefinition() const;
  [[nodiscard]] TypeDefinitionClientCapabilities add_typeDefinition();

  // Capabilities specific to the `textDocument/implementation` request.
  // 
  // @since 3.6.0
  [[nodiscard]] ImplementationClientCapabilities implementation();
  [[nodiscard]] bool has_implementation() const;
  [[nodiscard]] ImplementationClientCapabilities add_implementation();

  // Capabilities specific to the `textDocument/references` request.
  [[nodiscard]] ReferenceClientCapabilities references();
  [[nodiscard]] bool has_references() const;
  [[nodiscard]] ReferenceClientCapabilities add_references();

  // Capabilities specific to the `textDocument/documentHighlight` request.
  [[nodiscard]] DocumentHighlightClientCapabilities documentHighlight();
  [[nodiscard]] bool has_documentHighlight() const;
  [[nodiscard]] DocumentHighlightClientCapabilities add_documentHighlight();

  // Capabilities specific to the `textDocument/documentSymbol` request.
  [[nodiscard]] DocumentSymbolClientCapabilities documentSymbol();
  [[nodiscard]] bool has_documentSymbol() const;
  [[nodiscard]] DocumentSymbolClientCapabilities add_documentSymbol();

  // Capabilities specific to the `textDocument/codeAction` request.
  [[nodiscard]] CodeActionClientCapabilities codeAction();
  [[nodiscard]] bool has_codeAction() const;
  [[nodiscard]] CodeActionClientCapabilities add_codeAction();

  // Capabilities specific to the `textDocument/codeLens` request.
  [[nodiscard]] CodeLensClientCapabilities codeLens();
  [[nodiscard]] bool has_codeLens() const;
  [[nodiscard]] CodeLensClientCapabilities add_codeLens();

  // Capabilities specific to the `textDocument/documentLink` request.
  [[nodiscard]] DocumentLinkClientCapabilities documentLink();
  [[nodiscard]] bool has_documentLink() const;
  [[nodiscard]] DocumentLinkClientCapabilities add_documentLink();

  // Capabilities specific to the `textDocument/documentColor` and the
  // `textDocument/colorPresentation` request.
  // 
  // @since 3.6.0
  [[nodiscard]] DocumentColorClientCapabilities colorProvider();
  [[nodiscard]] bool has_colorProvider() const;
  [[nodiscard]] DocumentColorClientCapabilities add_colorProvider();

  // Capabilities specific to the `textDocument/formatting` request.
  [[nodiscard]] DocumentFormattingClientCapabilities formatting();
  [[nodiscard]] bool has_formatting() const;
  [[nodiscard]] DocumentFormattingClientCapabilities add_formatting();

  // Capabilities specific to the `textDocument/rangeFormatting` request.
  [[nodiscard]] DocumentRangeFormattingClientCapabilities rangeFormatting();
  [[nodiscard]] bool has_rangeFormatting() const;
  [[nodiscard]] DocumentRangeFormattingClientCapabilities add_rangeFormatting();

  // Capabilities specific to the `textDocument/onTypeFormatting` request.
  [[nodiscard]] DocumentOnTypeFormattingClientCapabilities onTypeFormatting();
  [[nodiscard]] bool has_onTypeFormatting() const;
  [[nodiscard]] DocumentOnTypeFormattingClientCapabilities add_onTypeFormatting();

  // Capabilities specific to the `textDocument/rename` request.
  [[nodiscard]] RenameClientCapabilities rename();
  [[nodiscard]] bool has_rename() const;
  [[nodiscard]] RenameClientCapabilities add_rename();

  // Capabilities specific to the `textDocument/foldingRange` request.
  // 
  // @since 3.10.0
  [[nodiscard]] FoldingRangeClientCapabilities foldingRange();
  [[nodiscard]] bool has_foldingRange() const;
  [[nodiscard]] FoldingRangeClientCapabilities add_foldingRange();

  // Capabilities specific to the `textDocument/selectionRange` request.
  // 
  // @since 3.15.0
  [[nodiscard]] SelectionRangeClientCapabilities selectionRange();
  [[nodiscard]] bool has_selectionRange() const;
  [[nodiscard]] SelectionRangeClientCapabilities add_selectionRange();

  // Capabilities specific to the `textDocument/publishDiagnostics` notification.
  [[nodiscard]] PublishDiagnosticsClientCapabilities publishDiagnostics();
  [[nodiscard]] bool has_publishDiagnostics() const;
  [[nodiscard]] PublishDiagnosticsClientCapabilities add_publishDiagnostics();

  // Capabilities specific to the various call hierarchy requests.
  // 
  // @since 3.16.0
  [[nodiscard]] CallHierarchyClientCapabilities callHierarchy();
  [[nodiscard]] bool has_callHierarchy() const;
  [[nodiscard]] CallHierarchyClientCapabilities add_callHierarchy();

  // Capabilities specific to the various semantic token request.
  // 
  // @since 3.16.0
  [[nodiscard]] SemanticTokensClientCapabilities semanticTokens();
  [[nodiscard]] bool has_semanticTokens() const;
  [[nodiscard]] SemanticTokensClientCapabilities add_semanticTokens();

  // Capabilities specific to the `textDocument/linkedEditingRange` request.
  // 
  // @since 3.16.0
  [[nodiscard]] LinkedEditingRangeClientCapabilities linkedEditingRange();
  [[nodiscard]] bool has_linkedEditingRange() const;
  [[nodiscard]] LinkedEditingRangeClientCapabilities add_linkedEditingRange();

  // Client capabilities specific to the `textDocument/moniker` request.
  // 
  // @since 3.16.0
  [[nodiscard]] MonikerClientCapabilities moniker();
  [[nodiscard]] bool has_moniker() const;
  [[nodiscard]] MonikerClientCapabilities add_moniker();

  // Capabilities specific to the various type hierarchy requests.
  // 
  // @since 3.17.0
  [[nodiscard]] TypeHierarchyClientCapabilities typeHierarchy();
  [[nodiscard]] bool has_typeHierarchy() const;
  [[nodiscard]] TypeHierarchyClientCapabilities add_typeHierarchy();

  // Capabilities specific to the `textDocument/inlineValue` request.
  // 
  // @since 3.17.0
  [[nodiscard]] InlineValueClientCapabilities inlineValue();
  [[nodiscard]] bool has_inlineValue() const;
  [[nodiscard]] InlineValueClientCapabilities add_inlineValue();

  // Capabilities specific to the `textDocument/inlayHint` request.
  // 
  // @since 3.17.0
  [[nodiscard]] InlayHintClientCapabilities inlayHint();
  [[nodiscard]] bool has_inlayHint() const;
  [[nodiscard]] InlayHintClientCapabilities add_inlayHint();

  // Capabilities specific to the diagnostic pull model.
  // 
  // @since 3.17.0
  [[nodiscard]] DiagnosticClientCapabilities diagnostic();
  [[nodiscard]] bool has_diagnostic() const;
  [[nodiscard]] DiagnosticClientCapabilities add_diagnostic();

  // Client capabilities specific to inline completions.
  // 
  // @since 3.18.0
  // @proposed
  [[nodiscard]] InlineCompletionClientCapabilities inlineCompletion();
  [[nodiscard]] bool has_inlineCompletion() const;
  [[nodiscard]] InlineCompletionClientCapabilities add_inlineCompletion();

  void init();
};

/**
 * The params sent in an open notebook document notification.
 * 
 * @since 3.17.0
 */
struct DidOpenNotebookDocumentParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The notebook document that got opened.
  [[nodiscard]] NotebookDocument notebookDocument();

  // The text documents that represent the content
  // of a notebook cell.
  [[nodiscard]] List<TextDocumentItem> cellTextDocuments();

  void init();
};

/**
 * Structural changes to cells in a notebook document.
 * 
 * @since 3.18.0
 */
struct NotebookDocumentCellChangeStructure : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The change to the cell array.
  [[nodiscard]] NotebookCellArrayChange array();

  // Additional opened cell text documents.
  [[nodiscard]] List<TextDocumentItem> didOpen();
  [[nodiscard]] bool has_didOpen() const;
  [[nodiscard]] List<TextDocumentItem> add_didOpen();

  // Additional closed cell text documents.
  [[nodiscard]] List<TextDocumentIdentifier> didClose();
  [[nodiscard]] bool has_didClose() const;
  [[nodiscard]] List<TextDocumentIdentifier> add_didClose();

  void init();
};

/**
 * Describe options to be used when registered for text document change events.
 */
struct DidChangeWatchedFilesRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The watchers to register.
  [[nodiscard]] List<FileSystemWatcher> watchers();

  void init();
};

/**
 * A document filter denotes a document by different properties like
 * the {@link TextDocument.languageId language}, the {@link Uri.scheme scheme} of
 * its resource, or a glob-pattern that is applied to the {@link TextDocument.fileName path}.
 * 
 * Glob patterns can have the following syntax:
 * - `*` to match one or more characters in a path segment
 * - `?` to match on one character in a path segment
 * - `**` to match any number of path segments, including none
 * - `{}` to group sub patterns into an OR expression. (e.g. `**​/\*.{ts,js}` matches all TypeScript and JavaScript files)
 * - `[]` to declare a range of characters to match in a path segment (e.g., `example.[0-9]` to match on `example.0`, `example.1`, …)
 * - `[!...]` to negate a range of characters to match in a path segment (e.g., `example.[!0-9]` to match on `example.a`, `example.b`, but not `example.0`)
 * 
 * @sample A language filter that applies to typescript files on disk: `{ language: 'typescript', scheme: 'file' }`
 * @sample A language filter that applies to all package.json paths: `{ language: 'json', pattern: '**package.json' }`
 * 
 * @since 3.17.0
 */
struct TextDocumentFilter : detail::StructWrapper {
  using StructWrapper::StructWrapper;

};

/**
 * A notebook document filter denotes a notebook document by
 * different properties. The properties will be match
 * against the notebook's URI (same as with documents)
 * 
 * @since 3.17.0
 */
struct NotebookDocumentFilter : detail::StructWrapper {
  using StructWrapper::StructWrapper;

};

/**
 * Inlay hint information.
 * 
 * @since 3.17.0
 */
struct InlayHint : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The position of this hint.
  // 
  // If multiple hints have the same position, they will be shown in the order
  // they appear in the response.
  [[nodiscard]] Position position();

  // The label of this hint. A human readable string or an array of
  // InlayHintLabelPart label parts.
  // 
  // *Note* that neither the string nor the label part can be empty.
  struct Union1 : detail::UnionStructWrapper<std::string_view, List<InlayHintLabelPart>> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_array_InlayHintLabelPart() const;
    [[nodiscard]] List<InlayHintLabelPart> as_array_InlayHintLabelPart();

  };

  [[nodiscard]] InlayHint::Union1 label();

  // The kind of this hint. Can be omitted in which case the client
  // should fall back to a reasonable default.
  [[nodiscard]] InlayHintKind kind();
  [[nodiscard]] bool has_kind() const;
  void set_kind(InlayHintKind val);

  // Optional text edits that are performed when accepting this inlay hint.
  // 
  // *Note* that edits are expected to change the document so that the inlay
  // hint (or its nearest variant) is now part of the document and the inlay
  // hint itself is now obsolete.
  [[nodiscard]] List<TextEdit> textEdits();
  [[nodiscard]] bool has_textEdits() const;
  [[nodiscard]] List<TextEdit> add_textEdits();

  // The tooltip text when you hover over this item.
  struct Union4 : detail::UnionStructWrapper<std::string_view, MarkupContent> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_MarkupContent() const;
    [[nodiscard]] MarkupContent as_MarkupContent();

  };

  [[nodiscard]] InlayHint::Union4 tooltip();
  [[nodiscard]] bool has_tooltip() const;
  [[nodiscard]] InlayHint::Union4 add_tooltip();

  // Render padding before the hint.
  // 
  // Note: Padding should use the editor's background color, not the
  // background color of the hint itself. That means padding can be used
  // to visually align/separate an inlay hint.
  [[nodiscard]] bool paddingLeft();
  [[nodiscard]] bool has_paddingLeft() const;
  void set_paddingLeft(bool val);

  // Render padding after the hint.
  // 
  // Note: Padding should use the editor's background color, not the
  // background color of the hint itself. That means padding can be used
  // to visually align/separate an inlay hint.
  [[nodiscard]] bool paddingRight();
  [[nodiscard]] bool has_paddingRight() const;
  void set_paddingRight(bool val);

  // A data entry field that is preserved on an inlay hint between
  // a `textDocument/inlayHint` and a `inlayHint/resolve` request.
  struct Union7 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] InlayHint::Union7 data();
  [[nodiscard]] bool has_data() const;
  [[nodiscard]] InlayHint::Union7 add_data();

  void init();
};

/**
 * Represents a diagnostic, such as a compiler error or warning. Diagnostic objects
 * are only valid in the scope of a resource.
 */
struct Diagnostic : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The range at which the message applies
  [[nodiscard]] Range range();

  // The diagnostic's severity. To avoid interpretation mismatches when a
  // server is used with different clients it is highly recommended that servers
  // always provide a severity value.
  [[nodiscard]] DiagnosticSeverity severity();
  [[nodiscard]] bool has_severity() const;
  void set_severity(DiagnosticSeverity val);

  // The diagnostic's code, which usually appear in the user interface.
  struct Union2 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] Diagnostic::Union2 code();
  [[nodiscard]] bool has_code() const;
  [[nodiscard]] Diagnostic::Union2 add_code();

  // An optional property to describe the error code.
  // Requires the code field (above) to be present/not null.
  // 
  // @since 3.16.0
  [[nodiscard]] CodeDescription codeDescription();
  [[nodiscard]] bool has_codeDescription() const;
  [[nodiscard]] CodeDescription add_codeDescription();

  // A human-readable string describing the source of this
  // diagnostic, e.g. 'typescript' or 'super lint'. It usually
  // appears in the user interface.
  [[nodiscard]] std::string_view source();
  [[nodiscard]] bool has_source() const;
  void set_source(std::string_view val);

  // The diagnostic's message. It usually appears in the user interface
  [[nodiscard]] std::string_view message();
  void set_message(std::string_view val);

  // Additional metadata about the diagnostic.
  // 
  // @since 3.15.0
  [[nodiscard]] List<DiagnosticTag> tags();
  [[nodiscard]] bool has_tags() const;
  [[nodiscard]] List<DiagnosticTag> add_tags();

  // An array of related diagnostic information, e.g. when symbol-names within
  // a scope collide all definitions can be marked via this property.
  [[nodiscard]] List<DiagnosticRelatedInformation> relatedInformation();
  [[nodiscard]] bool has_relatedInformation() const;
  [[nodiscard]] List<DiagnosticRelatedInformation> add_relatedInformation();

  // A data entry field that is preserved between a `textDocument/publishDiagnostics`
  // notification and `textDocument/codeAction` request.
  // 
  // @since 3.16.0
  struct Union8 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] Diagnostic::Union8 data();
  [[nodiscard]] bool has_data() const;
  [[nodiscard]] Diagnostic::Union8 add_data();

  void init();
};

/**
 * Describes textual changes on a text document. A TextDocumentEdit describes all changes
 * on a document version Si and after they are applied move the document to version Si+1.
 * So the creator of a TextDocumentEdit doesn't need to sort the array of edits or do any
 * kind of ordering. However the edits must be non overlapping.
 */
struct TextDocumentEdit : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The text document to change.
  [[nodiscard]] OptionalVersionedTextDocumentIdentifier textDocument();

  // The edits to be applied.
  // 
  // @since 3.16.0 - support for AnnotatedTextEdit. This is guarded using a
  // client capability.
  // 
  // @since 3.18.0 - support for SnippetTextEdit. This is guarded using a
  // client capability.
  [[nodiscard]] List<std::variant<TextEdit, AnnotatedTextEdit, SnippetTextEdit>> edits();

  void init();
};

/**
 * A parameter literal used in inline completion requests.
 * 
 * @since 3.18.0
 * @proposed
 */
struct InlineCompletionParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Additional information about the context in which inline completions were
  // requested.
  [[nodiscard]] InlineCompletionContext context();

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  // An optional token that a server can use to report work done progress.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] InlineCompletionParams::Union3 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] InlineCompletionParams::Union3 add_workDoneToken();

  void init();
};

/**
 * The change text document notification's parameters.
 */
struct DidChangeTextDocumentParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document that did change. The version number points
  // to the version after all provided content changes have
  // been applied.
  [[nodiscard]] VersionedTextDocumentIdentifier textDocument();

  // The actual content changes. The content changes describe single state changes
  // to the document. So if there are two content changes c1 (at array index 0) and
  // c2 (at array index 1) for a document in state S then c1 moves the document from
  // S to S' and c2 from S' to S''. So c1 is computed on the state S and c2 is computed
  // on the state S'.
  // 
  // To mirror the content of a document using change events use the following approach:
  // - start with the same initial content
  // - apply the 'textDocument/didChange' notifications in the order you receive them.
  // - apply the `TextDocumentContentChangeEvent`s in a single notification in the order
  //   you receive them.
  [[nodiscard]] List<TextDocumentContentChangeEvent> contentChanges();

  void init();
};

/**
 * Content changes to a cell in a notebook document.
 * 
 * @since 3.18.0
 */
struct NotebookDocumentCellContentChanges : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] VersionedTextDocumentIdentifier document();
  [[nodiscard]] List<TextDocumentContentChangeEvent> changes();

  void init();
};

/**
 * Represents a collection of {@link CompletionItem completion items} to be presented
 * in the editor.
 */
struct CompletionList : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // This list it not complete. Further typing results in recomputing this list.
  // 
  // Recomputed lists have all their items replaced (not appended) in the
  // incomplete completion sessions.
  [[nodiscard]] bool isIncomplete();
  void set_isIncomplete(bool val);

  // In many cases the items of an actual completion result share the same
  // value for properties like `commitCharacters` or the range of a text
  // edit. A completion list can therefore define item defaults which will
  // be used if a completion item itself doesn't specify the value.
  // 
  // If a completion list specifies a default value and a completion item
  // also specifies a corresponding value, the rules for combining these are
  // defined by `applyKinds` (if the client supports it), defaulting to
  // ApplyKind.Replace.
  // 
  // Servers are only allowed to return default values if the client
  // signals support for this via the `completionList.itemDefaults`
  // capability.
  // 
  // @since 3.17.0
  [[nodiscard]] CompletionItemDefaults itemDefaults();
  [[nodiscard]] bool has_itemDefaults() const;
  [[nodiscard]] CompletionItemDefaults add_itemDefaults();

  // Specifies how fields from a completion item should be combined with those
  // from `completionList.itemDefaults`.
  // 
  // If unspecified, all fields will be treated as ApplyKind.Replace.
  // 
  // If a field's value is ApplyKind.Replace, the value from a completion item
  // (if provided and not `null`) will always be used instead of the value
  // from `completionItem.itemDefaults`.
  // 
  // If a field's value is ApplyKind.Merge, the values will be merged using
  // the rules defined against each field below.
  // 
  // Servers are only allowed to return `applyKind` if the client
  // signals support for this via the `completionList.applyKindSupport`
  // capability.
  // 
  // @since 3.18.0
  [[nodiscard]] CompletionItemApplyKinds applyKind();
  [[nodiscard]] bool has_applyKind() const;
  [[nodiscard]] CompletionItemApplyKinds add_applyKind();

  // The completion items.
  [[nodiscard]] List<CompletionItem> items();

  void init();
};

/**
 * Options for notifications/requests for user operations on files.
 * 
 * @since 3.16.0
 */
struct FileOperationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The server is interested in receiving didCreateFiles notifications.
  [[nodiscard]] FileOperationRegistrationOptions didCreate();
  [[nodiscard]] bool has_didCreate() const;
  [[nodiscard]] FileOperationRegistrationOptions add_didCreate();

  // The server is interested in receiving willCreateFiles requests.
  [[nodiscard]] FileOperationRegistrationOptions willCreate();
  [[nodiscard]] bool has_willCreate() const;
  [[nodiscard]] FileOperationRegistrationOptions add_willCreate();

  // The server is interested in receiving didRenameFiles notifications.
  [[nodiscard]] FileOperationRegistrationOptions didRename();
  [[nodiscard]] bool has_didRename() const;
  [[nodiscard]] FileOperationRegistrationOptions add_didRename();

  // The server is interested in receiving willRenameFiles requests.
  [[nodiscard]] FileOperationRegistrationOptions willRename();
  [[nodiscard]] bool has_willRename() const;
  [[nodiscard]] FileOperationRegistrationOptions add_willRename();

  // The server is interested in receiving didDeleteFiles file notifications.
  [[nodiscard]] FileOperationRegistrationOptions didDelete();
  [[nodiscard]] bool has_didDelete() const;
  [[nodiscard]] FileOperationRegistrationOptions add_didDelete();

  // The server is interested in receiving willDeleteFiles file requests.
  [[nodiscard]] FileOperationRegistrationOptions willDelete();
  [[nodiscard]] bool has_willDelete() const;
  [[nodiscard]] FileOperationRegistrationOptions add_willDelete();

  void init();
};

/**
 * Additional information about the context in which a signature help request was triggered.
 * 
 * @since 3.15.0
 */
struct SignatureHelpContext : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Action that caused signature help to be triggered.
  [[nodiscard]] SignatureHelpTriggerKind triggerKind();
  void set_triggerKind(SignatureHelpTriggerKind val);

  // Character that caused signature help to be triggered.
  // 
  // This is undefined when `triggerKind !== SignatureHelpTriggerKind.TriggerCharacter`
  [[nodiscard]] std::string_view triggerCharacter();
  [[nodiscard]] bool has_triggerCharacter() const;
  void set_triggerCharacter(std::string_view val);

  // `true` if signature help was already showing when it was triggered.
  // 
  // Retriggers occurs when the signature help is already active and can be caused by actions such as
  // typing a trigger character, a cursor move, or document content changes.
  [[nodiscard]] bool isRetrigger();
  void set_isRetrigger(bool val);

  // The currently active `SignatureHelp`.
  // 
  // The `activeSignatureHelp` has its `SignatureHelp.activeSignature` field updated based on
  // the user navigating through available signatures.
  [[nodiscard]] SignatureHelp activeSignatureHelp();
  [[nodiscard]] bool has_activeSignatureHelp() const;
  [[nodiscard]] SignatureHelp add_activeSignatureHelp();

  void init();
};

/**
 * Defines the capabilities provided by the client.
 */
struct ClientCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Workspace specific client capabilities.
  [[nodiscard]] WorkspaceClientCapabilities workspace();
  [[nodiscard]] bool has_workspace() const;
  [[nodiscard]] WorkspaceClientCapabilities add_workspace();

  // Text document specific client capabilities.
  [[nodiscard]] TextDocumentClientCapabilities textDocument();
  [[nodiscard]] bool has_textDocument() const;
  [[nodiscard]] TextDocumentClientCapabilities add_textDocument();

  // Capabilities specific to the notebook document support.
  // 
  // @since 3.17.0
  [[nodiscard]] NotebookDocumentClientCapabilities notebookDocument();
  [[nodiscard]] bool has_notebookDocument() const;
  [[nodiscard]] NotebookDocumentClientCapabilities add_notebookDocument();

  // Window specific client capabilities.
  [[nodiscard]] WindowClientCapabilities window();
  [[nodiscard]] bool has_window() const;
  [[nodiscard]] WindowClientCapabilities add_window();

  // General client capabilities.
  // 
  // @since 3.16.0
  [[nodiscard]] GeneralClientCapabilities general();
  [[nodiscard]] bool has_general() const;
  [[nodiscard]] GeneralClientCapabilities add_general();

  // Experimental client capabilities.
  struct Union5 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] ClientCapabilities::Union5 experimental();
  [[nodiscard]] bool has_experimental() const;
  [[nodiscard]] ClientCapabilities::Union5 add_experimental();

  void init();
};

/**
 * @since 3.18.0
 */
struct NotebookDocumentFilterWithNotebook : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The notebook to be synced If a string
  // value is provided it matches against the
  // notebook type. '*' matches every notebook.
  struct Union0 : detail::UnionStructWrapper<std::string_view, NotebookDocumentFilter> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    struct Union1 : detail::UnionStructWrapper<NotebookDocumentFilterNotebookType, NotebookDocumentFilterScheme, NotebookDocumentFilterPattern> {
      using UnionStructWrapper::UnionStructWrapper;

      [[nodiscard]] bool holds_NotebookDocumentFilterNotebookType() const;
      [[nodiscard]] NotebookDocumentFilterNotebookType as_NotebookDocumentFilterNotebookType();

      [[nodiscard]] bool holds_NotebookDocumentFilterScheme() const;
      [[nodiscard]] NotebookDocumentFilterScheme as_NotebookDocumentFilterScheme();

      [[nodiscard]] bool holds_NotebookDocumentFilterPattern() const;
      [[nodiscard]] NotebookDocumentFilterPattern as_NotebookDocumentFilterPattern();

    };

    [[nodiscard]] bool holds_reference_Union1() const;
    [[nodiscard]] NotebookDocumentFilter as_reference_Union1();

  };

  [[nodiscard]] NotebookDocumentFilterWithNotebook::Union0 notebook();

  // The cells of the matching notebook to be synced.
  [[nodiscard]] List<NotebookCellLanguage> cells();
  [[nodiscard]] bool has_cells() const;
  [[nodiscard]] List<NotebookCellLanguage> add_cells();

  void init();
};

/**
 * @since 3.18.0
 */
struct NotebookDocumentFilterWithCells : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The notebook to be synced If a string
  // value is provided it matches against the
  // notebook type. '*' matches every notebook.
  struct Union0 : detail::UnionStructWrapper<std::string_view, NotebookDocumentFilter> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    struct Union1 : detail::UnionStructWrapper<NotebookDocumentFilterNotebookType, NotebookDocumentFilterScheme, NotebookDocumentFilterPattern> {
      using UnionStructWrapper::UnionStructWrapper;

      [[nodiscard]] bool holds_NotebookDocumentFilterNotebookType() const;
      [[nodiscard]] NotebookDocumentFilterNotebookType as_NotebookDocumentFilterNotebookType();

      [[nodiscard]] bool holds_NotebookDocumentFilterScheme() const;
      [[nodiscard]] NotebookDocumentFilterScheme as_NotebookDocumentFilterScheme();

      [[nodiscard]] bool holds_NotebookDocumentFilterPattern() const;
      [[nodiscard]] NotebookDocumentFilterPattern as_NotebookDocumentFilterPattern();

    };

    [[nodiscard]] bool holds_reference_Union1() const;
    [[nodiscard]] NotebookDocumentFilter as_reference_Union1();

  };

  [[nodiscard]] NotebookDocumentFilterWithCells::Union0 notebook();
  [[nodiscard]] bool has_notebook() const;
  [[nodiscard]] NotebookDocumentFilterWithCells::Union0 add_notebook();

  // The cells of the matching notebook to be synced.
  [[nodiscard]] List<NotebookCellLanguage> cells();

  void init();
};

/**
 * A notebook cell text document filter denotes a cell text
 * document by different properties.
 * 
 * @since 3.17.0
 */
struct NotebookCellTextDocumentFilter : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A filter that matches against the notebook
  // containing the notebook cell. If a string
  // value is provided it matches against the
  // notebook type. '*' matches every notebook.
  struct Union0 : detail::UnionStructWrapper<std::string_view, NotebookDocumentFilter> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    struct Union1 : detail::UnionStructWrapper<NotebookDocumentFilterNotebookType, NotebookDocumentFilterScheme, NotebookDocumentFilterPattern> {
      using UnionStructWrapper::UnionStructWrapper;

      [[nodiscard]] bool holds_NotebookDocumentFilterNotebookType() const;
      [[nodiscard]] NotebookDocumentFilterNotebookType as_NotebookDocumentFilterNotebookType();

      [[nodiscard]] bool holds_NotebookDocumentFilterScheme() const;
      [[nodiscard]] NotebookDocumentFilterScheme as_NotebookDocumentFilterScheme();

      [[nodiscard]] bool holds_NotebookDocumentFilterPattern() const;
      [[nodiscard]] NotebookDocumentFilterPattern as_NotebookDocumentFilterPattern();

    };

    [[nodiscard]] bool holds_reference_Union1() const;
    [[nodiscard]] NotebookDocumentFilter as_reference_Union1();

  };

  [[nodiscard]] NotebookCellTextDocumentFilter::Union0 notebook();

  // A language id like `python`.
  // 
  // Will be matched against the language id of the
  // notebook cell document. '*' matches every language.
  [[nodiscard]] std::string_view language();
  [[nodiscard]] bool has_language() const;
  void set_language(std::string_view val);

  void init();
};

/**
 * The publish diagnostic notification's parameters.
 */
struct PublishDiagnosticsParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The URI for which diagnostic information is reported.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  // Optional the version number of the document the diagnostics are published for.
  // 
  // @since 3.15.0
  [[nodiscard]] std::int32_t version();
  [[nodiscard]] bool has_version() const;
  void set_version(std::int32_t val);

  // An array of diagnostic information items.
  [[nodiscard]] List<Diagnostic> diagnostics();

  void init();
};

/**
 * A diagnostic report with a full set of problems.
 * 
 * @since 3.17.0
 */
struct FullDocumentDiagnosticReport : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // An optional result id. If provided it will
  // be sent on the next diagnostic request for the
  // same document.
  [[nodiscard]] std::string_view resultId();
  [[nodiscard]] bool has_resultId() const;
  void set_resultId(std::string_view val);

  // The actual items.
  [[nodiscard]] List<Diagnostic> items();

  void init();
};

/**
 * Contains additional diagnostic information about the context in which
 * a {@link CodeActionProvider.provideCodeActions code action} is run.
 */
struct CodeActionContext : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // An array of diagnostics known on the client side overlapping the range provided to the
  // `textDocument/codeAction` request. They are provided so that the server knows which
  // errors are currently presented to the user for the given range. There is no guarantee
  // that these accurately reflect the error state of the resource. The primary parameter
  // to compute code actions is the provided range.
  [[nodiscard]] List<Diagnostic> diagnostics();

  // Requested kind of actions to return.
  // 
  // Actions not of this kind are filtered out by the client before being shown. So servers
  // can omit computing them.
  [[nodiscard]] List<CodeActionKind> only();
  [[nodiscard]] bool has_only() const;
  [[nodiscard]] List<CodeActionKind> add_only();

  // The reason why code actions were requested.
  // 
  // @since 3.17.0
  [[nodiscard]] CodeActionTriggerKind triggerKind();
  [[nodiscard]] bool has_triggerKind() const;
  void set_triggerKind(CodeActionTriggerKind val);

  void init();
};

/**
 * A workspace edit represents changes to many resources managed in the workspace. The edit
 * should either provide `changes` or `documentChanges`. If documentChanges are present
 * they are preferred over `changes` if the client can handle versioned document edits.
 * 
 * Since version 3.13.0 a workspace edit can contain resource operations as well. If resource
 * operations are present clients need to execute the operations in the order in which they
 * are provided. So a workspace edit for example can consist of the following two changes:
 * (1) a create file a.txt and (2) a text document edit which insert text into file a.txt.
 * 
 * An invalid sequence (e.g. (1) delete file a.txt and (2) insert text into file a.txt) will
 * cause failure of the operation. How the client recovers from the failure is described by
 * the client capability: `workspace.workspaceEdit.failureHandling`
 */
struct WorkspaceEdit : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Holds changes to existing resources.
  [[nodiscard]] Dict<std::string_view, List<TextEdit>> changes();
  [[nodiscard]] bool has_changes() const;
  [[nodiscard]] Dict<std::string_view, List<TextEdit>> add_changes();

  // Depending on the client capability `workspace.workspaceEdit.resourceOperations` document changes
  // are either an array of `TextDocumentEdit`s to express changes to n different text documents
  // where each text document edit addresses a specific version of a text document. Or it can contain
  // above `TextDocumentEdit`s mixed with create, rename and delete file / folder operations.
  // 
  // Whether a client supports versioned document edits is expressed via
  // `workspace.workspaceEdit.documentChanges` client capability.
  // 
  // If a client neither supports `documentChanges` nor `workspace.workspaceEdit.resourceOperations` then
  // only plain `TextEdit`s using the `changes` property are supported.
  [[nodiscard]] List<std::variant<TextDocumentEdit, CreateFile, RenameFile, DeleteFile>> documentChanges();
  [[nodiscard]] bool has_documentChanges() const;
  [[nodiscard]] List<std::variant<TextDocumentEdit, CreateFile, RenameFile, DeleteFile>> add_documentChanges();

  // A map of change annotations that can be referenced in `AnnotatedTextEdit`s or create, rename and
  // delete file / folder operations.
  // 
  // Whether clients honor this property depends on the client capability `workspace.changeAnnotationSupport`.
  // 
  // @since 3.16.0
  [[nodiscard]] Dict<ChangeAnnotationIdentifier, ChangeAnnotation> changeAnnotations();
  [[nodiscard]] bool has_changeAnnotations() const;
  [[nodiscard]] Dict<ChangeAnnotationIdentifier, ChangeAnnotation> add_changeAnnotations();

  void init();
};

/**
 * Cell changes to a notebook document.
 * 
 * @since 3.18.0
 */
struct NotebookDocumentCellChanges : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Changes to the cell structure to add or
  // remove cells.
  [[nodiscard]] NotebookDocumentCellChangeStructure structure();
  [[nodiscard]] bool has_structure() const;
  [[nodiscard]] NotebookDocumentCellChangeStructure add_structure();

  // Changes to notebook cells properties like its
  // kind, execution summary or metadata.
  [[nodiscard]] List<NotebookCell> data();
  [[nodiscard]] bool has_data() const;
  [[nodiscard]] List<NotebookCell> add_data();

  // Changes to the text content of notebook cells.
  [[nodiscard]] List<NotebookDocumentCellContentChanges> textContent();
  [[nodiscard]] bool has_textContent() const;
  [[nodiscard]] List<NotebookDocumentCellContentChanges> add_textContent();

  void init();
};

/**
 * Defines workspace specific capabilities of the server.
 * 
 * @since 3.18.0
 */
struct WorkspaceOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The server supports workspace folder.
  // 
  // @since 3.6.0
  [[nodiscard]] WorkspaceFoldersServerCapabilities workspaceFolders();
  [[nodiscard]] bool has_workspaceFolders() const;
  [[nodiscard]] WorkspaceFoldersServerCapabilities add_workspaceFolders();

  // The server is interested in notifications/requests for operations on files.
  // 
  // @since 3.16.0
  [[nodiscard]] FileOperationOptions fileOperations();
  [[nodiscard]] bool has_fileOperations() const;
  [[nodiscard]] FileOperationOptions add_fileOperations();

  // The server supports the `workspace/textDocumentContent` request.
  // 
  // @since 3.18.0
  // @proposed
  struct Union2 : detail::UnionStructWrapper<TextDocumentContentOptions, TextDocumentContentRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_TextDocumentContentOptions() const;
    [[nodiscard]] TextDocumentContentOptions as_TextDocumentContentOptions();

    [[nodiscard]] bool holds_TextDocumentContentRegistrationOptions() const;
    [[nodiscard]] TextDocumentContentRegistrationOptions as_TextDocumentContentRegistrationOptions();

  };

  [[nodiscard]] WorkspaceOptions::Union2 textDocumentContent();
  [[nodiscard]] bool has_textDocumentContent() const;
  [[nodiscard]] WorkspaceOptions::Union2 add_textDocumentContent();

  void init();
};

/**
 * Parameters for a {@link SignatureHelpRequest}.
 */
struct SignatureHelpParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The signature help context. This is only available if the client specifies
  // to send this using the client capability `textDocument.signatureHelp.contextSupport === true`
  // 
  // @since 3.15.0
  [[nodiscard]] SignatureHelpContext context();
  [[nodiscard]] bool has_context() const;
  [[nodiscard]] SignatureHelpContext add_context();

  // The text document.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The position inside the text document.
  [[nodiscard]] Position position();

  // An optional token that a server can use to report work done progress.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] SignatureHelpParams::Union3 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] SignatureHelpParams::Union3 add_workDoneToken();

  void init();
};

/**
 * The initialize parameters
 */
struct _InitializeParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The process Id of the parent process that started
  // the server.
  // 
  // Is `null` if the process has not been started by another process.
  // If the parent process is not alive then the server should exit.
  struct Union0 : detail::UnionStructWrapper<std::int32_t> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

  };

  [[nodiscard]] _InitializeParams::Union0 processId();

  // Information about the client
  // 
  // @since 3.15.0
  [[nodiscard]] ClientInfo clientInfo();
  [[nodiscard]] bool has_clientInfo() const;
  [[nodiscard]] ClientInfo add_clientInfo();

  // The locale the client is currently showing the user interface
  // in. This must not necessarily be the locale of the operating
  // system.
  // 
  // Uses IETF language tags as the value's syntax
  // (See https://en.wikipedia.org/wiki/IETF_language_tag)
  // 
  // @since 3.16.0
  [[nodiscard]] std::string_view locale();
  [[nodiscard]] bool has_locale() const;
  void set_locale(std::string_view val);

  // The rootPath of the workspace. Is null
  // if no folder is open.
  // 
  // @deprecated in favour of rootUri.
  struct Union3 : detail::UnionStructWrapper<std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] _InitializeParams::Union3 rootPath();
  [[nodiscard]] bool has_rootPath() const;
  [[nodiscard]] _InitializeParams::Union3 add_rootPath();

  // The rootUri of the workspace. Is null if no
  // folder is open. If both `rootPath` and `rootUri` are set
  // `rootUri` wins.
  // 
  // @deprecated in favour of workspaceFolders.
  struct Union4 : detail::UnionStructWrapper<std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_DocumentUri() const;
    [[nodiscard]] std::string_view as_DocumentUri();
    void set_DocumentUri(std::string_view val);

  };

  [[nodiscard]] _InitializeParams::Union4 rootUri();

  // The capabilities provided by the client (editor or tool)
  [[nodiscard]] ClientCapabilities capabilities();

  // User provided initialization options.
  struct Union6 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] _InitializeParams::Union6 initializationOptions();
  [[nodiscard]] bool has_initializationOptions() const;
  [[nodiscard]] _InitializeParams::Union6 add_initializationOptions();

  // The initial trace setting. If omitted trace is disabled ('off').
  [[nodiscard]] TraceValue trace();
  [[nodiscard]] bool has_trace() const;
  void set_trace(TraceValue val);

  // An optional token that a server can use to report work done progress.
  struct Union8 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] _InitializeParams::Union8 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] _InitializeParams::Union8 add_workDoneToken();

  void init();
};

/**
 * Options specific to a notebook plus its cells
 * to be synced to the server.
 * 
 * If a selector provides a notebook document
 * filter but no cell selector all cells of a
 * matching notebook document will be synced.
 * 
 * If a selector provides no notebook document
 * filter but only a cell selector all notebook
 * document that contain at least one matching
 * cell will be synced.
 * 
 * @since 3.17.0
 */
struct NotebookDocumentSyncOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The notebooks to be synced
  [[nodiscard]] List<std::variant<NotebookDocumentFilterWithNotebook, NotebookDocumentFilterWithCells>> notebookSelector();

  // Whether save notification should be forwarded to
  // the server. Will only be honored if mode === `notebook`.
  [[nodiscard]] bool save();
  [[nodiscard]] bool has_save() const;
  void set_save(bool val);

  void init();
};

/**
 * A document filter describes a top level text document or
 * a notebook cell document.
 * 
 * @since 3.17.0 - support for NotebookCellTextDocumentFilter.
 */
struct DocumentFilter : detail::StructWrapper {
  using StructWrapper::StructWrapper;

};

/**
 * A partial result for a document diagnostic report.
 * 
 * @since 3.17.0
 */
struct DocumentDiagnosticReportPartialResult : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] Dict<std::string_view, std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>> relatedDocuments();

  void init();
};

/**
 * A full diagnostic report with a set of related documents.
 * 
 * @since 3.17.0
 */
struct RelatedFullDocumentDiagnosticReport : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Diagnostics of related documents. This information is useful
  // in programming languages where code in a file A can generate
  // diagnostics in a file B which A depends on. An example of
  // such a language is C/C++ where marco definitions in a file
  // a.cpp and result in errors in a header file b.hpp.
  // 
  // @since 3.17.0
  [[nodiscard]] Dict<std::string_view, std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>> relatedDocuments();
  [[nodiscard]] bool has_relatedDocuments() const;
  [[nodiscard]] Dict<std::string_view, std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>> add_relatedDocuments();

  // An optional result id. If provided it will
  // be sent on the next diagnostic request for the
  // same document.
  [[nodiscard]] std::string_view resultId();
  [[nodiscard]] bool has_resultId() const;
  void set_resultId(std::string_view val);

  // The actual items.
  [[nodiscard]] List<Diagnostic> items();

  void init();
};

/**
 * An unchanged diagnostic report with a set of related documents.
 * 
 * @since 3.17.0
 */
struct RelatedUnchangedDocumentDiagnosticReport : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // Diagnostics of related documents. This information is useful
  // in programming languages where code in a file A can generate
  // diagnostics in a file B which A depends on. An example of
  // such a language is C/C++ where marco definitions in a file
  // a.cpp and result in errors in a header file b.hpp.
  // 
  // @since 3.17.0
  [[nodiscard]] Dict<std::string_view, std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>> relatedDocuments();
  [[nodiscard]] bool has_relatedDocuments() const;
  [[nodiscard]] Dict<std::string_view, std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>> add_relatedDocuments();

  // A result id which will be sent on the next
  // diagnostic request for the same document.
  [[nodiscard]] std::string_view resultId();
  void set_resultId(std::string_view val);

  void init();
};

/**
 * A full document diagnostic report for a workspace diagnostic result.
 * 
 * @since 3.17.0
 */
struct WorkspaceFullDocumentDiagnosticReport : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The URI for which diagnostic information is reported.
  [[nodiscard]] std::string_view uri();
  void set_uri(std::string_view val);

  // The version number for which the diagnostics are reported.
  // If the document is not marked as open `null` can be provided.
  struct Union1 : detail::UnionStructWrapper<std::int32_t> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

  };

  [[nodiscard]] WorkspaceFullDocumentDiagnosticReport::Union1 version();

  // An optional result id. If provided it will
  // be sent on the next diagnostic request for the
  // same document.
  [[nodiscard]] std::string_view resultId();
  [[nodiscard]] bool has_resultId() const;
  void set_resultId(std::string_view val);

  // The actual items.
  [[nodiscard]] List<Diagnostic> items();

  void init();
};

/**
 * The parameters of a {@link CodeActionRequest}.
 */
struct CodeActionParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The document in which the command was invoked.
  [[nodiscard]] TextDocumentIdentifier textDocument();

  // The range for which the command was invoked.
  [[nodiscard]] Range range();

  // Context carrying additional information.
  [[nodiscard]] CodeActionContext context();

  // An optional token that a server can use to report work done progress.
  struct Union3 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] CodeActionParams::Union3 workDoneToken();
  [[nodiscard]] bool has_workDoneToken() const;
  [[nodiscard]] CodeActionParams::Union3 add_workDoneToken();

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  struct Union4 : detail::UnionStructWrapper<std::int32_t, std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] CodeActionParams::Union4 partialResultToken();
  [[nodiscard]] bool has_partialResultToken() const;
  [[nodiscard]] CodeActionParams::Union4 add_partialResultToken();

  void init();
};

/**
 * A code action represents a change that can be performed in code, e.g. to fix a problem or
 * to refactor code.
 * 
 * A CodeAction must set either `edit` and/or a `command`. If both are supplied, the `edit` is applied first, then the `command` is executed.
 */
struct CodeAction : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A short, human-readable, title for this code action.
  [[nodiscard]] std::string_view title();
  void set_title(std::string_view val);

  // The kind of the code action.
  // 
  // Used to filter code actions.
  [[nodiscard]] CodeActionKind kind();
  [[nodiscard]] bool has_kind() const;
  void set_kind(CodeActionKind val);

  // The diagnostics that this code action resolves.
  [[nodiscard]] List<Diagnostic> diagnostics();
  [[nodiscard]] bool has_diagnostics() const;
  [[nodiscard]] List<Diagnostic> add_diagnostics();

  // Marks this as a preferred action. Preferred actions are used by the `auto fix` command and can be targeted
  // by keybindings.
  // 
  // A quick fix should be marked preferred if it properly addresses the underlying error.
  // A refactoring should be marked preferred if it is the most reasonable choice of actions to take.
  // 
  // @since 3.15.0
  [[nodiscard]] bool isPreferred();
  [[nodiscard]] bool has_isPreferred() const;
  void set_isPreferred(bool val);

  // Marks that the code action cannot currently be applied.
  // 
  // Clients should follow the following guidelines regarding disabled code actions:
  // 
  //   - Disabled code actions are not shown in automatic [lightbulbs](https://code.visualstudio.com/docs/editor/editingevolved#_code-action)
  //     code action menus.
  // 
  //   - Disabled actions are shown as faded out in the code action menu when the user requests a more specific type
  //     of code action, such as refactorings.
  // 
  //   - If the user has a [keybinding](https://code.visualstudio.com/docs/editor/refactoring#_keybindings-for-code-actions)
  //     that auto applies a code action and only disabled code actions are returned, the client should show the user an
  //     error message with `reason` in the editor.
  // 
  // @since 3.16.0
  [[nodiscard]] CodeActionDisabled disabled();
  [[nodiscard]] bool has_disabled() const;
  [[nodiscard]] CodeActionDisabled add_disabled();

  // The workspace edit this code action performs.
  [[nodiscard]] WorkspaceEdit edit();
  [[nodiscard]] bool has_edit() const;
  [[nodiscard]] WorkspaceEdit add_edit();

  // A command this code action executes. If a code action
  // provides an edit and a command, first the edit is
  // executed and then the command.
  [[nodiscard]] Command command();
  [[nodiscard]] bool has_command() const;
  [[nodiscard]] Command add_command();

  // A data entry field that is preserved on a code action between
  // a `textDocument/codeAction` and a `codeAction/resolve` request.
  // 
  // @since 3.16.0
  struct Union7 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] CodeAction::Union7 data();
  [[nodiscard]] bool has_data() const;
  [[nodiscard]] CodeAction::Union7 add_data();

  // Tags for this code action.
  // 
  // @since 3.18.0 - proposed
  [[nodiscard]] List<CodeActionTag> tags();
  [[nodiscard]] bool has_tags() const;
  [[nodiscard]] List<CodeActionTag> add_tags();

  void init();
};

/**
 * The parameters passed via an apply workspace edit request.
 */
struct ApplyWorkspaceEditParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // An optional label of the workspace edit. This label is
  // presented in the user interface for example on an undo
  // stack to undo the workspace edit.
  [[nodiscard]] std::string_view label();
  [[nodiscard]] bool has_label() const;
  void set_label(std::string_view val);

  // The edits to apply.
  [[nodiscard]] WorkspaceEdit edit();

  // Additional data about the edit.
  // 
  // @since 3.18.0
  // @proposed
  [[nodiscard]] WorkspaceEditMetadata metadata();
  [[nodiscard]] bool has_metadata() const;
  [[nodiscard]] WorkspaceEditMetadata add_metadata();

  void init();
};

/**
 * A change event for a notebook document.
 * 
 * @since 3.17.0
 */
struct NotebookDocumentChangeEvent : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The changed meta data if any.
  // 
  // Note: should always be an object literal (e.g. LSPObject)
  [[nodiscard]] LSPObject metadata();
  [[nodiscard]] bool has_metadata() const;
  [[nodiscard]] LSPObject add_metadata();

  // Changes to cells
  [[nodiscard]] NotebookDocumentCellChanges cells();
  [[nodiscard]] bool has_cells() const;
  [[nodiscard]] NotebookDocumentCellChanges add_cells();

  void init();
};

struct InitializeParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The process Id of the parent process that started
  // the server.
  // 
  // Is `null` if the process has not been started by another process.
  // If the parent process is not alive then the server should exit.
  struct Union0 : detail::UnionStructWrapper<std::int32_t> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

  };

  [[nodiscard]] InitializeParams::Union0 processId();

  // Information about the client
  // 
  // @since 3.15.0
  [[nodiscard]] ClientInfo clientInfo();
  [[nodiscard]] bool has_clientInfo() const;
  [[nodiscard]] ClientInfo add_clientInfo();

  // The locale the client is currently showing the user interface
  // in. This must not necessarily be the locale of the operating
  // system.
  // 
  // Uses IETF language tags as the value's syntax
  // (See https://en.wikipedia.org/wiki/IETF_language_tag)
  // 
  // @since 3.16.0
  [[nodiscard]] std::string_view locale();
  [[nodiscard]] bool has_locale() const;
  void set_locale(std::string_view val);

  // The rootPath of the workspace. Is null
  // if no folder is open.
  // 
  // @deprecated in favour of rootUri.
  struct Union3 : detail::UnionStructWrapper<std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

  };

  [[nodiscard]] InitializeParams::Union3 rootPath();
  [[nodiscard]] bool has_rootPath() const;
  [[nodiscard]] InitializeParams::Union3 add_rootPath();

  // The rootUri of the workspace. Is null if no
  // folder is open. If both `rootPath` and `rootUri` are set
  // `rootUri` wins.
  // 
  // @deprecated in favour of workspaceFolders.
  struct Union4 : detail::UnionStructWrapper<std::string_view> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_DocumentUri() const;
    [[nodiscard]] std::string_view as_DocumentUri();
    void set_DocumentUri(std::string_view val);

  };

  [[nodiscard]] InitializeParams::Union4 rootUri();

  // The capabilities provided by the client (editor or tool)
  [[nodiscard]] ClientCapabilities capabilities();

  // User provided initialization options.
  struct Union6 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] InitializeParams::Union6 initializationOptions();
  [[nodiscard]] bool has_initializationOptions() const;
  [[nodiscard]] InitializeParams::Union6 add_initializationOptions();

  // The initial trace setting. If omitted trace is disabled ('off').
  [[nodiscard]] TraceValue trace();
  [[nodiscard]] bool has_trace() const;
  void set_trace(TraceValue val);

  // The workspace folders configured in the client when the server starts.
  // 
  // This property is only available if the client supports workspace folders.
  // It can be `null` if the client supports workspace folders but none are
  // configured.
  // 
  // @since 3.6.0
  struct Union8 : detail::UnionStructWrapper<List<WorkspaceFolder>> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_array_WorkspaceFolder() const;
    [[nodiscard]] List<WorkspaceFolder> as_array_WorkspaceFolder();

  };

  [[nodiscard]] InitializeParams::Union8 workspaceFolders();
  [[nodiscard]] bool has_workspaceFolders() const;
  [[nodiscard]] InitializeParams::Union8 add_workspaceFolders();

  void init();
};

/**
 * Registration options specific to a notebook.
 * 
 * @since 3.17.0
 */
struct NotebookDocumentSyncRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The notebooks to be synced
  [[nodiscard]] List<std::variant<NotebookDocumentFilterWithNotebook, NotebookDocumentFilterWithCells>> notebookSelector();

  // Whether save notification should be forwarded to
  // the server. Will only be honored if mode === `notebook`.
  [[nodiscard]] bool save();
  [[nodiscard]] bool has_save() const;
  void set_save(bool val);

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

/**
 * A document selector is the combination of one or many document filters.
 * 
 * @sample `let sel:DocumentSelector = [{ language: 'typescript' }, { language: 'json', pattern: '**∕tsconfig.json' }]`;
 * 
 * The use of a string as a document filter is deprecated @since 3.16.0.
 */
using DocumentSelector = List<DocumentFilter>;

/**
 * The result of a document diagnostic pull request. A report can
 * either be a full report containing all diagnostics for the
 * requested document or an unchanged report indicating that nothing
 * has changed in terms of diagnostics in comparison to the last
 * pull request.
 * 
 * @since 3.17.0
 */
struct DocumentDiagnosticReport : detail::StructWrapper {
  using StructWrapper::StructWrapper;

};

/**
 * A workspace diagnostic document report.
 * 
 * @since 3.17.0
 */
struct WorkspaceDocumentDiagnosticReport : detail::StructWrapper {
  using StructWrapper::StructWrapper;

};

/**
 * The params sent in a change notebook document notification.
 * 
 * @since 3.17.0
 */
struct DidChangeNotebookDocumentParams : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The notebook document that did change. The version number points
  // to the version after all provided changes have been applied. If
  // only the text document content of a cell changes the notebook version
  // doesn't necessarily have to change.
  [[nodiscard]] VersionedNotebookDocumentIdentifier notebookDocument();

  // The actual changes to the notebook document.
  // 
  // The changes describe single state changes to the notebook document.
  // So if there are two changes c1 (at array index 0) and c2 (at array
  // index 1) for a notebook in state S then c1 moves the notebook from
  // S to S' and c2 from S' to S''. So c1 is computed on the state S and
  // c2 is computed on the state S'.
  // 
  // To mirror the content of a notebook using change events use the following approach:
  // - start with the same initial content
  // - apply the 'notebookDocument/didChange' notifications in the order you receive them.
  // - apply the `NotebookChangeEvent`s in a single notification in the order
  //   you receive them.
  [[nodiscard]] NotebookDocumentChangeEvent change();

  void init();
};

/**
 * General text document registration options.
 */
struct TextDocumentRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] TextDocumentRegistrationOptions::Union0 documentSelector();

  void init();
};

/**
 * A workspace diagnostic report.
 * 
 * @since 3.17.0
 */
struct WorkspaceDiagnosticReport : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] List<WorkspaceDocumentDiagnosticReport> items();

  void init();
};

/**
 * A partial result for a workspace diagnostic report.
 * 
 * @since 3.17.0
 */
struct WorkspaceDiagnosticReportPartialResult : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  [[nodiscard]] List<WorkspaceDocumentDiagnosticReport> items();

  void init();
};

struct ImplementationRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] ImplementationRegistrationOptions::Union0 documentSelector();

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

struct TypeDefinitionRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] TypeDefinitionRegistrationOptions::Union0 documentSelector();

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

struct DocumentColorRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] DocumentColorRegistrationOptions::Union0 documentSelector();

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

struct FoldingRangeRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] FoldingRangeRegistrationOptions::Union0 documentSelector();

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

struct DeclarationRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] DeclarationRegistrationOptions::Union0 documentSelector();

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

struct SelectionRangeRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] SelectionRangeRegistrationOptions::Union0 documentSelector();

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

/**
 * Call hierarchy options used during static or dynamic registration.
 * 
 * @since 3.16.0
 */
struct CallHierarchyRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] CallHierarchyRegistrationOptions::Union0 documentSelector();

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

/**
 * @since 3.16.0
 */
struct SemanticTokensRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] SemanticTokensRegistrationOptions::Union0 documentSelector();

  // The legend used by the server
  [[nodiscard]] SemanticTokensLegend legend();

  // Server supports providing semantic tokens for a specific range
  // of a document.
  struct Union2 : detail::UnionStructWrapper<bool, LiteralStub> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] SemanticTokensRegistrationOptions::Union2 range();
  [[nodiscard]] bool has_range() const;
  [[nodiscard]] SemanticTokensRegistrationOptions::Union2 add_range();

  // Server supports providing semantic tokens for a full document.
  struct Union3 : detail::UnionStructWrapper<bool, SemanticTokensFullDelta> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_SemanticTokensFullDelta() const;
    [[nodiscard]] SemanticTokensFullDelta as_SemanticTokensFullDelta();

  };

  [[nodiscard]] SemanticTokensRegistrationOptions::Union3 full();
  [[nodiscard]] bool has_full() const;
  [[nodiscard]] SemanticTokensRegistrationOptions::Union3 add_full();

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

struct LinkedEditingRangeRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] LinkedEditingRangeRegistrationOptions::Union0 documentSelector();

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

struct MonikerRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] MonikerRegistrationOptions::Union0 documentSelector();

  void init();
};

/**
 * Type hierarchy options used during static or dynamic registration.
 * 
 * @since 3.17.0
 */
struct TypeHierarchyRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] TypeHierarchyRegistrationOptions::Union0 documentSelector();

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

/**
 * Inline value options used during static or dynamic registration.
 * 
 * @since 3.17.0
 */
struct InlineValueRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] InlineValueRegistrationOptions::Union0 documentSelector();

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

/**
 * Inlay hint options used during static or dynamic registration.
 * 
 * @since 3.17.0
 */
struct InlayHintRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The server provides support to resolve additional
  // information for an inlay hint item.
  [[nodiscard]] bool resolveProvider();
  [[nodiscard]] bool has_resolveProvider() const;
  void set_resolveProvider(bool val);

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union1 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] InlayHintRegistrationOptions::Union1 documentSelector();

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

/**
 * Diagnostic registration options.
 * 
 * @since 3.17.0
 */
struct DiagnosticRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] DiagnosticRegistrationOptions::Union0 documentSelector();

  // An optional identifier under which the diagnostics are
  // managed by the client.
  [[nodiscard]] std::string_view identifier();
  [[nodiscard]] bool has_identifier() const;
  void set_identifier(std::string_view val);

  // Whether the language has inter file dependencies meaning that
  // editing code in one file can result in a different diagnostic
  // set in another file. Inter file dependencies are common for
  // most programming languages and typically uncommon for linters.
  [[nodiscard]] bool interFileDependencies();
  void set_interFileDependencies(bool val);

  // The server provides support for workspace diagnostics as well.
  [[nodiscard]] bool workspaceDiagnostics();
  void set_workspaceDiagnostics(bool val);

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

/**
 * Inline completion options used during static or dynamic registration.
 * 
 * @since 3.18.0
 * @proposed
 */
struct InlineCompletionRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] InlineCompletionRegistrationOptions::Union0 documentSelector();

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  [[nodiscard]] std::string_view id();
  [[nodiscard]] bool has_id() const;
  void set_id(std::string_view val);

  void init();
};

/**
 * Describe options to be used when registered for text document change events.
 */
struct TextDocumentChangeRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // How documents are synced to the server.
  [[nodiscard]] TextDocumentSyncKind syncKind();
  void set_syncKind(TextDocumentSyncKind val);

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union1 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] TextDocumentChangeRegistrationOptions::Union1 documentSelector();

  void init();
};

/**
 * Save registration options.
 */
struct TextDocumentSaveRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] TextDocumentSaveRegistrationOptions::Union0 documentSelector();

  // The client is supposed to include the content on save.
  [[nodiscard]] bool includeText();
  [[nodiscard]] bool has_includeText() const;
  void set_includeText(bool val);

  void init();
};

/**
 * Registration options for a {@link CompletionRequest}.
 */
struct CompletionRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] CompletionRegistrationOptions::Union0 documentSelector();

  // Most tools trigger completion request automatically without explicitly requesting
  // it using a keyboard shortcut (e.g. Ctrl+Space). Typically they do so when the user
  // starts to type an identifier. For example if the user types `c` in a JavaScript file
  // code complete will automatically pop up present `console` besides others as a
  // completion item. Characters that make up identifiers don't need to be listed here.
  // 
  // If code complete should automatically be trigger on characters not being valid inside
  // an identifier (for example `.` in JavaScript) list them in `triggerCharacters`.
  [[nodiscard]] List<std::string_view> triggerCharacters();
  [[nodiscard]] bool has_triggerCharacters() const;
  [[nodiscard]] List<std::string_view> add_triggerCharacters();

  // The list of all possible characters that commit a completion. This field can be used
  // if clients don't support individual commit characters per completion item. See
  // `ClientCapabilities.textDocument.completion.completionItem.commitCharactersSupport`
  // 
  // If a server provides both `allCommitCharacters` and commit characters on an individual
  // completion item the ones on the completion item win.
  // 
  // @since 3.2.0
  [[nodiscard]] List<std::string_view> allCommitCharacters();
  [[nodiscard]] bool has_allCommitCharacters() const;
  [[nodiscard]] List<std::string_view> add_allCommitCharacters();

  // The server provides support to resolve additional
  // information for a completion item.
  [[nodiscard]] bool resolveProvider();
  [[nodiscard]] bool has_resolveProvider() const;
  void set_resolveProvider(bool val);

  // The server supports the following `CompletionItem` specific
  // capabilities.
  // 
  // @since 3.17.0
  [[nodiscard]] ServerCompletionItemOptions completionItem();
  [[nodiscard]] bool has_completionItem() const;
  [[nodiscard]] ServerCompletionItemOptions add_completionItem();

  void init();
};

/**
 * Registration options for a {@link HoverRequest}.
 */
struct HoverRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] HoverRegistrationOptions::Union0 documentSelector();

  void init();
};

/**
 * Registration options for a {@link SignatureHelpRequest}.
 */
struct SignatureHelpRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] SignatureHelpRegistrationOptions::Union0 documentSelector();

  // List of characters that trigger signature help automatically.
  [[nodiscard]] List<std::string_view> triggerCharacters();
  [[nodiscard]] bool has_triggerCharacters() const;
  [[nodiscard]] List<std::string_view> add_triggerCharacters();

  // List of characters that re-trigger signature help.
  // 
  // These trigger characters are only active when signature help is already showing. All trigger characters
  // are also counted as re-trigger characters.
  // 
  // @since 3.15.0
  [[nodiscard]] List<std::string_view> retriggerCharacters();
  [[nodiscard]] bool has_retriggerCharacters() const;
  [[nodiscard]] List<std::string_view> add_retriggerCharacters();

  void init();
};

/**
 * Registration options for a {@link DefinitionRequest}.
 */
struct DefinitionRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] DefinitionRegistrationOptions::Union0 documentSelector();

  void init();
};

/**
 * Registration options for a {@link ReferencesRequest}.
 */
struct ReferenceRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] ReferenceRegistrationOptions::Union0 documentSelector();

  void init();
};

/**
 * Registration options for a {@link DocumentHighlightRequest}.
 */
struct DocumentHighlightRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] DocumentHighlightRegistrationOptions::Union0 documentSelector();

  void init();
};

/**
 * Registration options for a {@link DocumentSymbolRequest}.
 */
struct DocumentSymbolRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] DocumentSymbolRegistrationOptions::Union0 documentSelector();

  // A human-readable string that is shown when multiple outlines trees
  // are shown for the same document.
  // 
  // @since 3.16.0
  [[nodiscard]] std::string_view label();
  [[nodiscard]] bool has_label() const;
  void set_label(std::string_view val);

  void init();
};

/**
 * Registration options for a {@link CodeActionRequest}.
 */
struct CodeActionRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] CodeActionRegistrationOptions::Union0 documentSelector();

  // CodeActionKinds that this server may return.
  // 
  // The list of kinds may be generic, such as `CodeActionKind.Refactor`, or the server
  // may list out every specific kind they provide.
  [[nodiscard]] List<CodeActionKind> codeActionKinds();
  [[nodiscard]] bool has_codeActionKinds() const;
  [[nodiscard]] List<CodeActionKind> add_codeActionKinds();

  // Static documentation for a class of code actions.
  // 
  // Documentation from the provider should be shown in the code actions menu if either:
  // 
  // - Code actions of `kind` are requested by the editor. In this case, the editor will show the documentation that
  //   most closely matches the requested code action kind. For example, if a provider has documentation for
  //   both `Refactor` and `RefactorExtract`, when the user requests code actions for `RefactorExtract`,
  //   the editor will use the documentation for `RefactorExtract` instead of the documentation for `Refactor`.
  // 
  // - Any code actions of `kind` are returned by the provider.
  // 
  // At most one documentation entry should be shown per provider.
  // 
  // @since 3.18.0
  // @proposed
  [[nodiscard]] List<CodeActionKindDocumentation> documentation();
  [[nodiscard]] bool has_documentation() const;
  [[nodiscard]] List<CodeActionKindDocumentation> add_documentation();

  // The server provides support to resolve additional
  // information for a code action.
  // 
  // @since 3.16.0
  [[nodiscard]] bool resolveProvider();
  [[nodiscard]] bool has_resolveProvider() const;
  void set_resolveProvider(bool val);

  void init();
};

/**
 * Registration options for a {@link CodeLensRequest}.
 */
struct CodeLensRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] CodeLensRegistrationOptions::Union0 documentSelector();

  // Code lens has a resolve provider as well.
  [[nodiscard]] bool resolveProvider();
  [[nodiscard]] bool has_resolveProvider() const;
  void set_resolveProvider(bool val);

  void init();
};

/**
 * Registration options for a {@link DocumentLinkRequest}.
 */
struct DocumentLinkRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] DocumentLinkRegistrationOptions::Union0 documentSelector();

  // Document links have a resolve provider as well.
  [[nodiscard]] bool resolveProvider();
  [[nodiscard]] bool has_resolveProvider() const;
  void set_resolveProvider(bool val);

  void init();
};

/**
 * Registration options for a {@link DocumentFormattingRequest}.
 */
struct DocumentFormattingRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] DocumentFormattingRegistrationOptions::Union0 documentSelector();

  void init();
};

/**
 * Registration options for a {@link DocumentRangeFormattingRequest}.
 */
struct DocumentRangeFormattingRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] DocumentRangeFormattingRegistrationOptions::Union0 documentSelector();

  // Whether the server supports formatting multiple ranges at once.
  // 
  // @since 3.18.0
  // @proposed
  [[nodiscard]] bool rangesSupport();
  [[nodiscard]] bool has_rangesSupport() const;
  void set_rangesSupport(bool val);

  void init();
};

/**
 * Registration options for a {@link DocumentOnTypeFormattingRequest}.
 */
struct DocumentOnTypeFormattingRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] DocumentOnTypeFormattingRegistrationOptions::Union0 documentSelector();

  // A character on which formatting should be triggered, like `{`.
  [[nodiscard]] std::string_view firstTriggerCharacter();
  void set_firstTriggerCharacter(std::string_view val);

  // More trigger characters.
  [[nodiscard]] List<std::string_view> moreTriggerCharacter();
  [[nodiscard]] bool has_moreTriggerCharacter() const;
  [[nodiscard]] List<std::string_view> add_moreTriggerCharacter();

  void init();
};

/**
 * Registration options for a {@link RenameRequest}.
 */
struct RenameRegistrationOptions : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  struct Union0 : detail::UnionStructWrapper<DocumentSelector> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_DocumentFilter() const;
    [[nodiscard]] DocumentSelector as_reference_DocumentFilter();

  };

  [[nodiscard]] RenameRegistrationOptions::Union0 documentSelector();

  // Renames should be checked and tested before being executed.
  // 
  // @since version 3.12.0
  [[nodiscard]] bool prepareProvider();
  [[nodiscard]] bool has_prepareProvider() const;
  void set_prepareProvider(bool val);

  void init();
};

/**
 * Defines the capabilities provided by a language
 * server.
 */
struct ServerCapabilities : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The position encoding the server picked from the encodings offered
  // by the client via the client capability `general.positionEncodings`.
  // 
  // If the client didn't provide any position encodings the only valid
  // value that a server can return is 'utf-16'.
  // 
  // If omitted it defaults to 'utf-16'.
  // 
  // @since 3.17.0
  [[nodiscard]] PositionEncodingKind positionEncoding();
  [[nodiscard]] bool has_positionEncoding() const;
  void set_positionEncoding(PositionEncodingKind val);

  // Defines how text documents are synced. Is either a detailed structure
  // defining each notification or for backwards compatibility the
  // TextDocumentSyncKind number.
  struct Union1 : detail::UnionStructWrapper<TextDocumentSyncOptions, TextDocumentSyncKind> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_TextDocumentSyncOptions() const;
    [[nodiscard]] TextDocumentSyncOptions as_TextDocumentSyncOptions();

    [[nodiscard]] bool holds_TextDocumentSyncKind() const;
    [[nodiscard]] TextDocumentSyncKind as_TextDocumentSyncKind();

  };

  [[nodiscard]] ServerCapabilities::Union1 textDocumentSync();
  [[nodiscard]] bool has_textDocumentSync() const;
  [[nodiscard]] ServerCapabilities::Union1 add_textDocumentSync();

  // Defines how notebook documents are synced.
  // 
  // @since 3.17.0
  struct Union2 : detail::UnionStructWrapper<NotebookDocumentSyncOptions, NotebookDocumentSyncRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_NotebookDocumentSyncOptions() const;
    [[nodiscard]] NotebookDocumentSyncOptions as_NotebookDocumentSyncOptions();

    [[nodiscard]] bool holds_NotebookDocumentSyncRegistrationOptions() const;
    [[nodiscard]] NotebookDocumentSyncRegistrationOptions as_NotebookDocumentSyncRegistrationOptions();

  };

  [[nodiscard]] ServerCapabilities::Union2 notebookDocumentSync();
  [[nodiscard]] bool has_notebookDocumentSync() const;
  [[nodiscard]] ServerCapabilities::Union2 add_notebookDocumentSync();

  // The server provides completion support.
  [[nodiscard]] CompletionOptions completionProvider();
  [[nodiscard]] bool has_completionProvider() const;
  [[nodiscard]] CompletionOptions add_completionProvider();

  // The server provides hover support.
  struct Union4 : detail::UnionStructWrapper<bool, HoverOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_HoverOptions() const;
    [[nodiscard]] HoverOptions as_HoverOptions();

  };

  [[nodiscard]] ServerCapabilities::Union4 hoverProvider();
  [[nodiscard]] bool has_hoverProvider() const;
  [[nodiscard]] ServerCapabilities::Union4 add_hoverProvider();

  // The server provides signature help support.
  [[nodiscard]] SignatureHelpOptions signatureHelpProvider();
  [[nodiscard]] bool has_signatureHelpProvider() const;
  [[nodiscard]] SignatureHelpOptions add_signatureHelpProvider();

  // The server provides Goto Declaration support.
  struct Union6 : detail::UnionStructWrapper<bool, DeclarationOptions, DeclarationRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_DeclarationOptions() const;
    [[nodiscard]] DeclarationOptions as_DeclarationOptions();

    [[nodiscard]] bool holds_DeclarationRegistrationOptions() const;
    [[nodiscard]] DeclarationRegistrationOptions as_DeclarationRegistrationOptions();

  };

  [[nodiscard]] ServerCapabilities::Union6 declarationProvider();
  [[nodiscard]] bool has_declarationProvider() const;
  [[nodiscard]] ServerCapabilities::Union6 add_declarationProvider();

  // The server provides goto definition support.
  struct Union7 : detail::UnionStructWrapper<bool, DefinitionOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_DefinitionOptions() const;
    [[nodiscard]] DefinitionOptions as_DefinitionOptions();

  };

  [[nodiscard]] ServerCapabilities::Union7 definitionProvider();
  [[nodiscard]] bool has_definitionProvider() const;
  [[nodiscard]] ServerCapabilities::Union7 add_definitionProvider();

  // The server provides Goto Type Definition support.
  struct Union8 : detail::UnionStructWrapper<bool, TypeDefinitionOptions, TypeDefinitionRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_TypeDefinitionOptions() const;
    [[nodiscard]] TypeDefinitionOptions as_TypeDefinitionOptions();

    [[nodiscard]] bool holds_TypeDefinitionRegistrationOptions() const;
    [[nodiscard]] TypeDefinitionRegistrationOptions as_TypeDefinitionRegistrationOptions();

  };

  [[nodiscard]] ServerCapabilities::Union8 typeDefinitionProvider();
  [[nodiscard]] bool has_typeDefinitionProvider() const;
  [[nodiscard]] ServerCapabilities::Union8 add_typeDefinitionProvider();

  // The server provides Goto Implementation support.
  struct Union9 : detail::UnionStructWrapper<bool, ImplementationOptions, ImplementationRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_ImplementationOptions() const;
    [[nodiscard]] ImplementationOptions as_ImplementationOptions();

    [[nodiscard]] bool holds_ImplementationRegistrationOptions() const;
    [[nodiscard]] ImplementationRegistrationOptions as_ImplementationRegistrationOptions();

  };

  [[nodiscard]] ServerCapabilities::Union9 implementationProvider();
  [[nodiscard]] bool has_implementationProvider() const;
  [[nodiscard]] ServerCapabilities::Union9 add_implementationProvider();

  // The server provides find references support.
  struct Union10 : detail::UnionStructWrapper<bool, ReferenceOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_ReferenceOptions() const;
    [[nodiscard]] ReferenceOptions as_ReferenceOptions();

  };

  [[nodiscard]] ServerCapabilities::Union10 referencesProvider();
  [[nodiscard]] bool has_referencesProvider() const;
  [[nodiscard]] ServerCapabilities::Union10 add_referencesProvider();

  // The server provides document highlight support.
  struct Union11 : detail::UnionStructWrapper<bool, DocumentHighlightOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_DocumentHighlightOptions() const;
    [[nodiscard]] DocumentHighlightOptions as_DocumentHighlightOptions();

  };

  [[nodiscard]] ServerCapabilities::Union11 documentHighlightProvider();
  [[nodiscard]] bool has_documentHighlightProvider() const;
  [[nodiscard]] ServerCapabilities::Union11 add_documentHighlightProvider();

  // The server provides document symbol support.
  struct Union12 : detail::UnionStructWrapper<bool, DocumentSymbolOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_DocumentSymbolOptions() const;
    [[nodiscard]] DocumentSymbolOptions as_DocumentSymbolOptions();

  };

  [[nodiscard]] ServerCapabilities::Union12 documentSymbolProvider();
  [[nodiscard]] bool has_documentSymbolProvider() const;
  [[nodiscard]] ServerCapabilities::Union12 add_documentSymbolProvider();

  // The server provides code actions. CodeActionOptions may only be
  // specified if the client states that it supports
  // `codeActionLiteralSupport` in its initial `initialize` request.
  struct Union13 : detail::UnionStructWrapper<bool, CodeActionOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_CodeActionOptions() const;
    [[nodiscard]] CodeActionOptions as_CodeActionOptions();

  };

  [[nodiscard]] ServerCapabilities::Union13 codeActionProvider();
  [[nodiscard]] bool has_codeActionProvider() const;
  [[nodiscard]] ServerCapabilities::Union13 add_codeActionProvider();

  // The server provides code lens.
  [[nodiscard]] CodeLensOptions codeLensProvider();
  [[nodiscard]] bool has_codeLensProvider() const;
  [[nodiscard]] CodeLensOptions add_codeLensProvider();

  // The server provides document link support.
  [[nodiscard]] DocumentLinkOptions documentLinkProvider();
  [[nodiscard]] bool has_documentLinkProvider() const;
  [[nodiscard]] DocumentLinkOptions add_documentLinkProvider();

  // The server provides color provider support.
  struct Union16 : detail::UnionStructWrapper<bool, DocumentColorOptions, DocumentColorRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_DocumentColorOptions() const;
    [[nodiscard]] DocumentColorOptions as_DocumentColorOptions();

    [[nodiscard]] bool holds_DocumentColorRegistrationOptions() const;
    [[nodiscard]] DocumentColorRegistrationOptions as_DocumentColorRegistrationOptions();

  };

  [[nodiscard]] ServerCapabilities::Union16 colorProvider();
  [[nodiscard]] bool has_colorProvider() const;
  [[nodiscard]] ServerCapabilities::Union16 add_colorProvider();

  // The server provides workspace symbol support.
  struct Union17 : detail::UnionStructWrapper<bool, WorkspaceSymbolOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_WorkspaceSymbolOptions() const;
    [[nodiscard]] WorkspaceSymbolOptions as_WorkspaceSymbolOptions();

  };

  [[nodiscard]] ServerCapabilities::Union17 workspaceSymbolProvider();
  [[nodiscard]] bool has_workspaceSymbolProvider() const;
  [[nodiscard]] ServerCapabilities::Union17 add_workspaceSymbolProvider();

  // The server provides document formatting.
  struct Union18 : detail::UnionStructWrapper<bool, DocumentFormattingOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_DocumentFormattingOptions() const;
    [[nodiscard]] DocumentFormattingOptions as_DocumentFormattingOptions();

  };

  [[nodiscard]] ServerCapabilities::Union18 documentFormattingProvider();
  [[nodiscard]] bool has_documentFormattingProvider() const;
  [[nodiscard]] ServerCapabilities::Union18 add_documentFormattingProvider();

  // The server provides document range formatting.
  struct Union19 : detail::UnionStructWrapper<bool, DocumentRangeFormattingOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_DocumentRangeFormattingOptions() const;
    [[nodiscard]] DocumentRangeFormattingOptions as_DocumentRangeFormattingOptions();

  };

  [[nodiscard]] ServerCapabilities::Union19 documentRangeFormattingProvider();
  [[nodiscard]] bool has_documentRangeFormattingProvider() const;
  [[nodiscard]] ServerCapabilities::Union19 add_documentRangeFormattingProvider();

  // The server provides document formatting on typing.
  [[nodiscard]] DocumentOnTypeFormattingOptions documentOnTypeFormattingProvider();
  [[nodiscard]] bool has_documentOnTypeFormattingProvider() const;
  [[nodiscard]] DocumentOnTypeFormattingOptions add_documentOnTypeFormattingProvider();

  // The server provides rename support. RenameOptions may only be
  // specified if the client states that it supports
  // `prepareSupport` in its initial `initialize` request.
  struct Union21 : detail::UnionStructWrapper<bool, RenameOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_RenameOptions() const;
    [[nodiscard]] RenameOptions as_RenameOptions();

  };

  [[nodiscard]] ServerCapabilities::Union21 renameProvider();
  [[nodiscard]] bool has_renameProvider() const;
  [[nodiscard]] ServerCapabilities::Union21 add_renameProvider();

  // The server provides folding provider support.
  struct Union22 : detail::UnionStructWrapper<bool, FoldingRangeOptions, FoldingRangeRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_FoldingRangeOptions() const;
    [[nodiscard]] FoldingRangeOptions as_FoldingRangeOptions();

    [[nodiscard]] bool holds_FoldingRangeRegistrationOptions() const;
    [[nodiscard]] FoldingRangeRegistrationOptions as_FoldingRangeRegistrationOptions();

  };

  [[nodiscard]] ServerCapabilities::Union22 foldingRangeProvider();
  [[nodiscard]] bool has_foldingRangeProvider() const;
  [[nodiscard]] ServerCapabilities::Union22 add_foldingRangeProvider();

  // The server provides selection range support.
  struct Union23 : detail::UnionStructWrapper<bool, SelectionRangeOptions, SelectionRangeRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_SelectionRangeOptions() const;
    [[nodiscard]] SelectionRangeOptions as_SelectionRangeOptions();

    [[nodiscard]] bool holds_SelectionRangeRegistrationOptions() const;
    [[nodiscard]] SelectionRangeRegistrationOptions as_SelectionRangeRegistrationOptions();

  };

  [[nodiscard]] ServerCapabilities::Union23 selectionRangeProvider();
  [[nodiscard]] bool has_selectionRangeProvider() const;
  [[nodiscard]] ServerCapabilities::Union23 add_selectionRangeProvider();

  // The server provides execute command support.
  [[nodiscard]] ExecuteCommandOptions executeCommandProvider();
  [[nodiscard]] bool has_executeCommandProvider() const;
  [[nodiscard]] ExecuteCommandOptions add_executeCommandProvider();

  // The server provides call hierarchy support.
  // 
  // @since 3.16.0
  struct Union25 : detail::UnionStructWrapper<bool, CallHierarchyOptions, CallHierarchyRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_CallHierarchyOptions() const;
    [[nodiscard]] CallHierarchyOptions as_CallHierarchyOptions();

    [[nodiscard]] bool holds_CallHierarchyRegistrationOptions() const;
    [[nodiscard]] CallHierarchyRegistrationOptions as_CallHierarchyRegistrationOptions();

  };

  [[nodiscard]] ServerCapabilities::Union25 callHierarchyProvider();
  [[nodiscard]] bool has_callHierarchyProvider() const;
  [[nodiscard]] ServerCapabilities::Union25 add_callHierarchyProvider();

  // The server provides linked editing range support.
  // 
  // @since 3.16.0
  struct Union26 : detail::UnionStructWrapper<bool, LinkedEditingRangeOptions, LinkedEditingRangeRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_LinkedEditingRangeOptions() const;
    [[nodiscard]] LinkedEditingRangeOptions as_LinkedEditingRangeOptions();

    [[nodiscard]] bool holds_LinkedEditingRangeRegistrationOptions() const;
    [[nodiscard]] LinkedEditingRangeRegistrationOptions as_LinkedEditingRangeRegistrationOptions();

  };

  [[nodiscard]] ServerCapabilities::Union26 linkedEditingRangeProvider();
  [[nodiscard]] bool has_linkedEditingRangeProvider() const;
  [[nodiscard]] ServerCapabilities::Union26 add_linkedEditingRangeProvider();

  // The server provides semantic tokens support.
  // 
  // @since 3.16.0
  struct Union27 : detail::UnionStructWrapper<SemanticTokensOptions, SemanticTokensRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_SemanticTokensOptions() const;
    [[nodiscard]] SemanticTokensOptions as_SemanticTokensOptions();

    [[nodiscard]] bool holds_SemanticTokensRegistrationOptions() const;
    [[nodiscard]] SemanticTokensRegistrationOptions as_SemanticTokensRegistrationOptions();

  };

  [[nodiscard]] ServerCapabilities::Union27 semanticTokensProvider();
  [[nodiscard]] bool has_semanticTokensProvider() const;
  [[nodiscard]] ServerCapabilities::Union27 add_semanticTokensProvider();

  // The server provides moniker support.
  // 
  // @since 3.16.0
  struct Union28 : detail::UnionStructWrapper<bool, MonikerOptions, MonikerRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_MonikerOptions() const;
    [[nodiscard]] MonikerOptions as_MonikerOptions();

    [[nodiscard]] bool holds_MonikerRegistrationOptions() const;
    [[nodiscard]] MonikerRegistrationOptions as_MonikerRegistrationOptions();

  };

  [[nodiscard]] ServerCapabilities::Union28 monikerProvider();
  [[nodiscard]] bool has_monikerProvider() const;
  [[nodiscard]] ServerCapabilities::Union28 add_monikerProvider();

  // The server provides type hierarchy support.
  // 
  // @since 3.17.0
  struct Union29 : detail::UnionStructWrapper<bool, TypeHierarchyOptions, TypeHierarchyRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_TypeHierarchyOptions() const;
    [[nodiscard]] TypeHierarchyOptions as_TypeHierarchyOptions();

    [[nodiscard]] bool holds_TypeHierarchyRegistrationOptions() const;
    [[nodiscard]] TypeHierarchyRegistrationOptions as_TypeHierarchyRegistrationOptions();

  };

  [[nodiscard]] ServerCapabilities::Union29 typeHierarchyProvider();
  [[nodiscard]] bool has_typeHierarchyProvider() const;
  [[nodiscard]] ServerCapabilities::Union29 add_typeHierarchyProvider();

  // The server provides inline values.
  // 
  // @since 3.17.0
  struct Union30 : detail::UnionStructWrapper<bool, InlineValueOptions, InlineValueRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_InlineValueOptions() const;
    [[nodiscard]] InlineValueOptions as_InlineValueOptions();

    [[nodiscard]] bool holds_InlineValueRegistrationOptions() const;
    [[nodiscard]] InlineValueRegistrationOptions as_InlineValueRegistrationOptions();

  };

  [[nodiscard]] ServerCapabilities::Union30 inlineValueProvider();
  [[nodiscard]] bool has_inlineValueProvider() const;
  [[nodiscard]] ServerCapabilities::Union30 add_inlineValueProvider();

  // The server provides inlay hints.
  // 
  // @since 3.17.0
  struct Union31 : detail::UnionStructWrapper<bool, InlayHintOptions, InlayHintRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_InlayHintOptions() const;
    [[nodiscard]] InlayHintOptions as_InlayHintOptions();

    [[nodiscard]] bool holds_InlayHintRegistrationOptions() const;
    [[nodiscard]] InlayHintRegistrationOptions as_InlayHintRegistrationOptions();

  };

  [[nodiscard]] ServerCapabilities::Union31 inlayHintProvider();
  [[nodiscard]] bool has_inlayHintProvider() const;
  [[nodiscard]] ServerCapabilities::Union31 add_inlayHintProvider();

  // The server has support for pull model diagnostics.
  // 
  // @since 3.17.0
  struct Union32 : detail::UnionStructWrapper<DiagnosticOptions, DiagnosticRegistrationOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_DiagnosticOptions() const;
    [[nodiscard]] DiagnosticOptions as_DiagnosticOptions();

    [[nodiscard]] bool holds_DiagnosticRegistrationOptions() const;
    [[nodiscard]] DiagnosticRegistrationOptions as_DiagnosticRegistrationOptions();

  };

  [[nodiscard]] ServerCapabilities::Union32 diagnosticProvider();
  [[nodiscard]] bool has_diagnosticProvider() const;
  [[nodiscard]] ServerCapabilities::Union32 add_diagnosticProvider();

  // Inline completion options used during static registration.
  // 
  // @since 3.18.0
  // @proposed
  struct Union33 : detail::UnionStructWrapper<bool, InlineCompletionOptions> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

    [[nodiscard]] bool holds_InlineCompletionOptions() const;
    [[nodiscard]] InlineCompletionOptions as_InlineCompletionOptions();

  };

  [[nodiscard]] ServerCapabilities::Union33 inlineCompletionProvider();
  [[nodiscard]] bool has_inlineCompletionProvider() const;
  [[nodiscard]] ServerCapabilities::Union33 add_inlineCompletionProvider();

  // Workspace specific server capabilities.
  [[nodiscard]] WorkspaceOptions workspace();
  [[nodiscard]] bool has_workspace() const;
  [[nodiscard]] WorkspaceOptions add_workspace();

  // Experimental server capabilities.
  struct Union35 : detail::UnionStructWrapper<LSPObject, LSPArray, std::string_view, std::int32_t, std::uint32_t, double, bool> {
    using UnionStructWrapper::UnionStructWrapper;

    [[nodiscard]] bool holds_reference_Map0() const;
    [[nodiscard]] LSPObject as_reference_Map0();

    [[nodiscard]] bool holds_reference_LSPAny() const;
    [[nodiscard]] LSPArray as_reference_LSPAny();

    [[nodiscard]] bool holds_string() const;
    [[nodiscard]] std::string_view as_string();
    void set_string(std::string_view val);

    [[nodiscard]] bool holds_integer() const;
    [[nodiscard]] std::int32_t as_integer();
    void set_integer(std::int32_t val);

    [[nodiscard]] bool holds_uinteger() const;
    [[nodiscard]] std::uint32_t as_uinteger();
    void set_uinteger(std::uint32_t val);

    [[nodiscard]] bool holds_decimal() const;
    [[nodiscard]] double as_decimal();
    void set_decimal(double val);

    [[nodiscard]] bool holds_boolean() const;
    [[nodiscard]] bool as_boolean();
    void set_boolean(bool val);

  };

  [[nodiscard]] ServerCapabilities::Union35 experimental();
  [[nodiscard]] bool has_experimental() const;
  [[nodiscard]] ServerCapabilities::Union35 add_experimental();

  void init();
};

/**
 * The result returned from an initialize request.
 */
struct InitializeResult : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  // The capabilities the language server provides.
  [[nodiscard]] ServerCapabilities capabilities();

  // Information about the server.
  // 
  // @since 3.15.0
  [[nodiscard]] ServerInfo serverInfo();
  [[nodiscard]] bool has_serverInfo() const;
  [[nodiscard]] ServerInfo add_serverInfo();

  void init();
};


}  // namespace lsp

// NOLINTEND(readability-identifier-naming)
