#pragma once

#include <cstdint>
#include <glaze/glaze.hpp>
#include <optional>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

// NOLINTBEGIN(readability-identifier-naming)

namespace lsp {

using LSPAny = glz::json_t;
using LSPArray = std::vector<LSPAny>;
using LSPObject = std::map<std::string_view, LSPAny>;
struct LiteralStub {};  // TODO

/**
 * A set of predefined token types. This set is not fixed
 * an clients can specify additional token types via the
 * corresponding client capabilities.
 *
 * @since 3.16.0
 */
enum class SemanticTokenTypes : std::uint8_t {
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
enum class SemanticTokenModifiers : std::uint8_t {
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
enum class DocumentDiagnosticReportKind : std::uint8_t {
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
enum class ErrorCodes : std::int32_t {
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

enum class LSPErrorCodes : std::int32_t {
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
enum class FoldingRangeKind : std::uint8_t {
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
enum class SymbolKind : std::uint8_t {
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
enum class SymbolTag : std::uint8_t {
  // Render a symbol as obsolete, usually using a strike-out.
  kDeprecated = 1,
};

/**
 * Moniker uniqueness level to define scope of the moniker.
 *
 * @since 3.16.0
 */
enum class UniquenessLevel : std::uint8_t {
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
enum class MonikerKind : std::uint8_t {
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
enum class InlayHintKind : std::uint8_t {
  // An inlay hint that for a type annotation.
  kType = 1,

  // An inlay hint that is for a parameter.
  kParameter = 2,
};

/**
 * The message type
 */
enum class MessageType : std::uint8_t {
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
enum class TextDocumentSyncKind : std::uint8_t {
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
enum class TextDocumentSaveReason : std::uint8_t {
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
enum class CompletionItemKind : std::uint8_t {
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
enum class CompletionItemTag : std::uint8_t {
  // Render a completion as obsolete, usually using a strike-out.
  kDeprecated = 1,
};

/**
 * Defines whether the insert text in a completion item should be interpreted as
 * plain text or a snippet.
 */
enum class InsertTextFormat : std::uint8_t {
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
enum class InsertTextMode : std::uint8_t {
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
enum class DocumentHighlightKind : std::uint8_t {
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
enum class CodeActionKind : std::uint8_t {
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
enum class CodeActionTag : std::uint8_t {
  // Marks the code action as LLM-generated.
  kLlmgenerated = 1,
};

enum class TraceValue : std::uint8_t {
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
enum class MarkupKind : std::uint8_t {
  // Plain text is supported as a content format
  kPlaintext,

  // Markdown is supported as a content format
  kMarkdown,
};

using LanguageKind = std::string_view;
/**
 * Describes how an {@link InlineCompletionItemProvider inline completion provider} was triggered.
 *
 * @since 3.18.0
 * @proposed
 */
enum class InlineCompletionTriggerKind : std::uint8_t {
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
enum class PositionEncodingKind : std::uint8_t {
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
enum class FileChangeType : std::uint8_t {
  // The file got created.
  kCreated = 1,

  // The file got changed.
  kChanged = 2,

  // The file got deleted.
  kDeleted = 3,
};

enum class WatchKind : std::uint8_t {
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
enum class DiagnosticSeverity : std::uint8_t {
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
enum class DiagnosticTag : std::uint8_t {
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
enum class CompletionTriggerKind : std::uint8_t {
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
enum class ApplyKind : std::uint8_t {
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
enum class SignatureHelpTriggerKind : std::uint8_t {
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
enum class CodeActionTriggerKind : std::uint8_t {
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
enum class FileOperationPatternKind : std::uint8_t {
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
enum class NotebookCellKind : std::uint8_t {
  // A markup-cell is formatted source that is used for display.
  kMarkup = 1,

  // A code-cell is source code.
  kCode = 2,
};

enum class ResourceOperationKind : std::uint8_t {
  // Supports creating new files and folders.
  kCreate,

  // Supports renaming existing files and folders.
  kRename,

  // Supports deleting existing files and folders.
  kDelete,
};

enum class FailureHandlingKind : std::uint8_t {
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

enum class PrepareSupportDefaultBehavior : std::uint8_t {
  // The client's default behavior is to select the identifier
  // according the to language's syntax rule.
  kIdentifier = 1,
};

enum class TokenFormat : std::uint8_t {
  kRelative,
};

/**
 * @since 3.18.0
 */
struct PrepareRenameDefaultBehavior {
  bool defaultBehavior;
};

using ProgressToken = std::variant<std::int32_t, std::string_view>;

/**
 * An identifier to refer to a change annotation stored with a workspace edit.
 */
using ChangeAnnotationIdentifier = std::string_view;

/**
 * @since 3.18.0
 */
struct TextDocumentContentChangeWholeDocument {
  // The new text of the whole document.
  std::string text;
};

/**
 * @since 3.18.0
 * @deprecated use MarkupContent instead.
 */
struct MarkedStringWithLanguage {
  std::string_view language;
  std::string_view value;
};

/**
 * The glob pattern to watch relative to the base path. Glob patterns can have the following syntax:
 * - `*` to match one or more characters in a path segment
 * - `?` to match on one character in a path segment
 * - `**` to match any number of path segments, including none
 * - `{}` to group conditions (e.g. `**​/\*.{ts,js}` matches all TypeScript and JavaScript files)
 * - `[]` to declare a range of characters to match in a path segment (e.g., `example.[0-9]` to match on `example.0`,
 * `example.1`, …)
 * - `[!...]` to negate a range of characters to match in a path segment (e.g., `example.[!0-9]` to match on
 * `example.a`, `example.b`, but not `example.0`)
 *
 * @since 3.17.0
 */
using Pattern = std::string_view;

using RegularExpressionEngineKind = std::string_view;

/**
 * Static registration options to be returned in the initialize
 * request.
 */
struct StaticRegistrationOptions {
  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

/**
 * A workspace folder inside a client.
 */
struct WorkspaceFolder {
  // The associated URI for this workspace folder.
  std::string_view uri;

  // The name of the workspace folder. Used to refer to this
  // workspace folder in the user interface.
  std::string_view name;
};

struct ConfigurationItem {
  // The scope to get the configuration section for.
  std::optional<std::string_view> scopeUri;

  // The configuration section asked for.
  std::optional<std::string_view> section;
};

/**
 * A literal to identify a text document in the client.
 */
struct TextDocumentIdentifier {
  // The text document's uri.
  std::string_view uri;
};

/**
 * Represents a color in RGBA space.
 */
struct Color {
  // The red component of this color in the range [0-1].
  double red;

  // The green component of this color in the range [0-1].
  double green;

  // The blue component of this color in the range [0-1].
  double blue;

  // The alpha component of this color in the range [0-1].
  double alpha;
};

struct WorkDoneProgressOptions {
  std::optional<bool> workDoneProgress;
};

/**
 * Position in a text document expressed as zero-based line and character
 * offset. Prior to 3.17 the offsets were always based on a UTF-16 string
 * representation. So a string of the form `a𐐀b` the character offset of the
 * character `a` is 0, the character offset of `𐐀` is 1 and the character
 * offset of b is 3 since `𐐀` is represented using two code units in UTF-16.
 * Since 3.17 clients and servers can agree on a different string encoding
 * representation (e.g. UTF-8). The client announces it's supported encoding
 * via the client capability
 * [`general.positionEncodings`](https://microsoft.github.io/language-server-protocol/specifications/specification-current/#clientCapabilities).
 * The value is an array of position encodings the client supports, with
 * decreasing preference (e.g. the encoding at index `0` is the most preferred
 * one). To stay backwards compatible the only mandatory encoding is UTF-16
 * represented via the string `utf-16`. The server can pick one of the
 * encodings offered by the client and signals that encoding back to the
 * client via the initialize result's property
 * [`capabilities.positionEncoding`](https://microsoft.github.io/language-server-protocol/specifications/specification-current/#serverCapabilities).
 * If the string value `utf-16` is missing from the client's capability `general.positionEncodings` servers can safely
 * assume that the client supports UTF-16. If the server omits the position encoding in its initialize result the
 * encoding defaults to the string value `utf-16`. Implementation considerations: since the conversion from one encoding
 * into another requires the content of the file / line the conversion is best done where the file is read which is
 * usually on the server side.
 *
 * Positions are line end character agnostic. So you can not specify a position
 * that denotes `\r|\n` or `\n|` where `|` represents the character offset.
 *
 * @since 3.17.0 - support for negotiated position encoding.
 */
struct Position {
  // Line position in a document (zero-based).
  std::uint32_t line;

  // Character offset on a line in a document (zero-based).
  //
  // The meaning of this offset is determined by the negotiated
  // `PositionEncodingKind`.
  std::uint32_t character;
};

/**
 * @since 3.16.0
 */
struct SemanticTokens {
  // An optional result id. If provided and clients support delta updating
  // the client will include the result id in the next semantic token request.
  // A server can then instead of computing all semantic tokens again simply
  // send a delta.
  std::optional<std::string_view> resultId;

  // The actual tokens.
  std::vector<std::uint32_t> data;
};

/**
 * @since 3.16.0
 */
struct SemanticTokensPartialResult {
  std::vector<std::uint32_t> data;
};

/**
 * @since 3.16.0
 */
struct SemanticTokensEdit {
  // The start offset of the edit.
  std::uint32_t start;

  // The count of elements to remove.
  std::uint32_t deleteCount;

  // The elements to insert.
  std::optional<std::vector<std::uint32_t>> data;
};

/**
 * The result of a showDocument request.
 *
 * @since 3.16.0
 */
struct ShowDocumentResult {
  // A boolean indicating if the show was successful.
  bool success;
};

/**
 * Represents information on a file/folder create.
 *
 * @since 3.16.0
 */
struct FileCreate {
  // A file:// URI for the location of the file/folder being created.
  std::string_view uri;
};

/**
 * Additional information that describes document changes.
 *
 * @since 3.16.0
 */
struct ChangeAnnotation {
  // A human-readable string describing the actual change. The string
  // is rendered prominent in the user interface.
  std::string_view label;

  // A flag which indicates that user confirmation is needed
  // before applying the change.
  std::optional<bool> needsConfirmation;

  // A human-readable string which is rendered less prominent in
  // the user interface.
  std::optional<std::string_view> description;
};

/**
 * Represents information on a file/folder rename.
 *
 * @since 3.16.0
 */
struct FileRename {
  // A file:// URI for the original location of the file/folder being renamed.
  std::string_view oldUri;

  // A file:// URI for the new location of the file/folder being renamed.
  std::string_view newUri;
};

/**
 * Represents information on a file/folder delete.
 *
 * @since 3.16.0
 */
struct FileDelete {
  // A file:// URI for the location of the file/folder being deleted.
  std::string_view uri;
};

/**
 * A diagnostic report indicating that the last returned
 * report is still accurate.
 *
 * @since 3.17.0
 */
struct UnchangedDocumentDiagnosticReport {
  // A document diagnostic report indicating
  // no changes to the last result. A server can
  // only return `unchanged` if result ids are
  // provided.
  const std::string_view kind;

  // A result id which will be sent on the next
  // diagnostic request for the same document.
  std::string_view resultId;
};

/**
 * Cancellation data returned from a diagnostic request.
 *
 * @since 3.17.0
 */
struct DiagnosticServerCancellationData {
  bool retriggerRequest;
};

/**
 * A previous result id in a workspace pull request.
 *
 * @since 3.17.0
 */
struct PreviousResultId {
  // The URI for which the client knowns a
  // result id.
  std::string_view uri;

  // The value of the previous result id.
  std::string_view value;
};

/**
 * A versioned notebook document identifier.
 *
 * @since 3.17.0
 */
struct VersionedNotebookDocumentIdentifier {
  // The version number of this notebook document.
  std::int32_t version;

  // The notebook document's uri.
  std::string_view uri;
};

/**
 * A literal to identify a notebook document in the client.
 *
 * @since 3.17.0
 */
struct NotebookDocumentIdentifier {
  // The notebook document's uri.
  std::string_view uri;
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
struct StringValue {
  // The kind of string value.
  const std::string_view kind;

  // The snippet string.
  std::string_view value;
};

/**
 * Parameters for the `workspace/textDocumentContent` request.
 *
 * @since 3.18.0
 * @proposed
 */
struct TextDocumentContentParams {
  // The uri of the text document.
  std::string_view uri;
};

/**
 * Result of the `workspace/textDocumentContent` request.
 *
 * @since 3.18.0
 * @proposed
 */
struct TextDocumentContentResult {
  // The text content of the text document. Please note, that the content of
  // any subsequent open notifications for the text document might differ
  // from the returned content due to whitespace and line ending
  // normalizations done on the client
  std::string text;
};

/**
 * Text document content provider options.
 *
 * @since 3.18.0
 * @proposed
 */
struct TextDocumentContentOptions {
  // The schemes for which the server provides content.
  std::vector<std::string_view> schemes;
};

/**
 * Parameters for the `workspace/textDocumentContent/refresh` request.
 *
 * @since 3.18.0
 * @proposed
 */
struct TextDocumentContentRefreshParams {
  // The uri of the text document to refresh.
  std::string_view uri;
};

/**
 * General parameters to unregister a request or notification.
 */
struct Unregistration {
  // The id used to unregister the request or notification. Usually an id
  // provided during the register request.
  std::string_view id;

  // The method to unregister for.
  std::string_view method;
};

/**
 * Information about the server
 *
 * @since 3.15.0
 * @since 3.18.0 ServerInfo type name added.
 */
struct ServerInfo {
  // The name of the server as defined by the server.
  std::string_view name;

  // The server's version as defined by the server.
  std::optional<std::string_view> version;
};

/**
 * The data type of the ResponseError if the
 * initialize request fails.
 */
struct InitializeError {
  // Indicates whether the client execute the following retry logic:
  // (1) show the message provided by the ResponseError to the user
  // (2) user selects retry or cancel
  // (3) if user selected retry the initialize method is sent again.
  bool retry;
};

struct InitializedParams {};

struct DidChangeConfigurationRegistrationOptions {
  std::optional<std::variant<std::string_view, std::vector<std::string_view>>> section;
};

struct MessageActionItem {
  // A short title like 'Retry', 'Open Log' etc.
  std::string_view title;
};

/**
 * Save options.
 */
struct SaveOptions {
  // The client is supposed to include the content on save.
  std::optional<bool> includeText;
};

/**
 * Additional details for a completion item label.
 *
 * @since 3.17.0
 */
struct CompletionItemLabelDetails {
  // An optional string which is rendered less prominently directly after {@link CompletionItem.label label},
  // without any spacing. Should be used for function signatures and type annotations.
  std::optional<std::string_view> detail;

  // An optional string which is rendered less prominently after {@link CompletionItem.detail}. Should be used
  // for fully qualified names and file paths.
  std::optional<std::string_view> description;
};

/**
 * Value-object that contains additional information when
 * requesting references.
 */
struct ReferenceContext {
  // Include the declaration of the current symbol.
  bool includeDeclaration;
};

/**
 * Captures why the code action is currently disabled.
 *
 * @since 3.18.0
 */
struct CodeActionDisabled {
  // Human readable description of why the code action is currently disabled.
  //
  // This is displayed in the code actions UI.
  std::string_view reason;
};

/**
 * Location with only uri and does not include range.
 *
 * @since 3.18.0
 */
struct LocationUriOnly {
  std::string_view uri;
};

/**
 * Value-object describing what options formatting should use.
 */
struct FormattingOptions {
  // Size of a tab in spaces.
  std::uint32_t tabSize;

  // Prefer spaces over tabs.
  bool insertSpaces;

  // Trim trailing whitespace on a line.
  //
  // @since 3.15.0
  std::optional<bool> trimTrailingWhitespace;

  // Insert a newline character at the end of the file if one does not exist.
  //
  // @since 3.15.0
  std::optional<bool> insertFinalNewline;

  // Trim all newlines after the final newline at the end of the file.
  //
  // @since 3.15.0
  std::optional<bool> trimFinalNewlines;
};

/**
 * Provider options for a {@link DocumentOnTypeFormattingRequest}.
 */
struct DocumentOnTypeFormattingOptions {
  // A character on which formatting should be triggered, like `{`.
  std::string_view firstTriggerCharacter;

  // More trigger characters.
  std::optional<std::vector<std::string_view>> moreTriggerCharacter;
};

/**
 * Additional data about a workspace edit.
 *
 * @since 3.18.0
 * @proposed
 */
struct WorkspaceEditMetadata {
  // Signal to the editor that this edit is a refactoring.
  std::optional<bool> isRefactoring;
};

/**
 * The result returned from the apply workspace edit request.
 *
 * @since 3.17 renamed from ApplyWorkspaceEditResponse
 */
struct ApplyWorkspaceEditResult {
  // Indicates whether the edit was applied or not.
  bool applied;

  // An optional textual description for why the edit was not applied.
  // This may be used by the server for diagnostic logging or to provide
  // a suitable error for a request that triggered the edit.
  std::optional<std::string_view> failureReason;

  // Depending on the client's failure handling strategy `failedChange` might
  // contain the index of the change that failed. This property is only available
  // if the client signals a `failureHandlingStrategy` in its client capabilities.
  std::optional<std::uint32_t> failedChange;
};

struct WorkDoneProgressBegin {
  const std::string_view kind;

  // Mandatory title of the progress operation. Used to briefly inform about
  // the kind of operation being performed.
  //
  // Examples: "Indexing" or "Linking dependencies".
  std::string_view title;

  // Controls if a cancel button should show to allow the user to cancel the
  // long running operation. Clients that don't support cancellation are allowed
  // to ignore the setting.
  std::optional<bool> cancellable;

  // Optional, more detailed associated progress message. Contains
  // complementary information to the `title`.
  //
  // Examples: "3/25 files", "project/src/module2", "node_modules/some_dep".
  // If unset, the previous progress message (if any) is still valid.
  std::optional<std::string_view> message;

  // Optional progress percentage to display (value 100 is considered 100%).
  // If not provided infinite progress is assumed and clients are allowed
  // to ignore the `percentage` value in subsequent in report notifications.
  //
  // The value should be steadily rising. Clients are free to ignore values
  // that are not following this rule. The value range is [0, 100].
  std::optional<std::uint32_t> percentage;
};

struct WorkDoneProgressReport {
  const std::string_view kind;

  // Controls enablement state of a cancel button.
  //
  // Clients that don't support cancellation or don't support controlling the button's
  // enablement state are allowed to ignore the property.
  std::optional<bool> cancellable;

  // Optional, more detailed associated progress message. Contains
  // complementary information to the `title`.
  //
  // Examples: "3/25 files", "project/src/module2", "node_modules/some_dep".
  // If unset, the previous progress message (if any) is still valid.
  std::optional<std::string_view> message;

  // Optional progress percentage to display (value 100 is considered 100%).
  // If not provided infinite progress is assumed and clients are allowed
  // to ignore the `percentage` value in subsequent in report notifications.
  //
  // The value should be steadily rising. Clients are free to ignore values
  // that are not following this rule. The value range is [0, 100]
  std::optional<std::uint32_t> percentage;
};

struct WorkDoneProgressEnd {
  const std::string_view kind;

  // Optional, a final message indicating to for example indicate the outcome
  // of the operation.
  std::optional<std::string_view> message;
};

struct LogTraceParams {
  std::string_view message;
  std::optional<std::string_view> verbose;
};

struct CancelParams {
  // The request id to cancel.
  std::variant<std::int32_t, std::string_view> id;
};

/**
 * @since 3.16.0
 */
struct SemanticTokensLegend {
  // The token types a server uses.
  std::vector<std::string_view> tokenTypes;

  // The token modifiers a server uses.
  std::vector<std::string_view> tokenModifiers;
};

/**
 * Semantic tokens options to support deltas for full documents
 *
 * @since 3.18.0
 */
struct SemanticTokensFullDelta {
  // The server supports deltas for full documents.
  std::optional<bool> delta;
};

/**
 * Options to create a file.
 */
struct CreateFileOptions {
  // Overwrite existing file. Overwrite wins over `ignoreIfExists`
  std::optional<bool> overwrite;

  // Ignore if exists.
  std::optional<bool> ignoreIfExists;
};

/**
 * Rename file options
 */
struct RenameFileOptions {
  // Overwrite target if existing. Overwrite wins over `ignoreIfExists`
  std::optional<bool> overwrite;

  // Ignores if target exists.
  std::optional<bool> ignoreIfExists;
};

/**
 * Delete file options
 */
struct DeleteFileOptions {
  // Delete the content recursively if a folder is denoted.
  std::optional<bool> recursive;

  // Ignore the operation if the file doesn't exist.
  std::optional<bool> ignoreIfNotExists;
};

/**
 * Information about the client
 *
 * @since 3.15.0
 * @since 3.18.0 ClientInfo type name added.
 */
struct ClientInfo {
  // The name of the client as defined by the client.
  std::string_view name;

  // The client's version as defined by the client.
  std::optional<std::string_view> version;
};

/**
 * Structure to capture a description for an error code.
 *
 * @since 3.16.0
 */
struct CodeDescription {
  // An URI to open with more information about the diagnostic error.
  std::string_view href;
};

/**
 * @since 3.18.0
 */
struct ServerCompletionItemOptions {
  // The server has support for completion item label
  // details (see also `CompletionItemLabelDetails`) when
  // receiving a completion item in a resolve call.
  //
  // @since 3.17.0
  std::optional<bool> labelDetailsSupport;
};

/**
 * Matching options for the file operation pattern.
 *
 * @since 3.16.0
 */
struct FileOperationPatternOptions {
  // The pattern should be matched ignoring casing.
  std::optional<bool> ignoreCase;
};

struct ExecutionSummary {
  // A strict monotonically increasing value
  // indicating the execution order of a cell
  // inside a notebook.
  std::uint32_t executionOrder;

  // Whether the execution was successful or
  // not if known by the client.
  std::optional<bool> success;
};

/**
 * @since 3.18.0
 */
struct NotebookCellLanguage {
  std::string_view language;
};

struct WorkspaceFoldersServerCapabilities {
  // The server has support for workspace folders
  std::optional<bool> supported;

  // Whether the server wants to receive workspace folder
  // change notifications.
  //
  // If a string is provided the string is treated as an ID
  // under which the notification is registered on the client
  // side. The ID can be used to unregister for these events
  // using the `client/unregisterCapability` request.
  std::optional<std::variant<std::string_view, bool>> changeNotifications;
};

struct DidChangeConfigurationClientCapabilities {
  // Did change configuration notification supports dynamic registration.
  std::optional<bool> dynamicRegistration;
};

struct DidChangeWatchedFilesClientCapabilities {
  // Did change watched files notification supports dynamic registration. Please note
  // that the current protocol doesn't support static configuration for file changes
  // from the server side.
  std::optional<bool> dynamicRegistration;

  // Whether the client has support for {@link  RelativePattern relative pattern}
  // or not.
  //
  // @since 3.17.0
  std::optional<bool> relativePatternSupport;
};

/**
 * The client capabilities of a {@link ExecuteCommandRequest}.
 */
struct ExecuteCommandClientCapabilities {
  // Execute command supports dynamic registration.
  std::optional<bool> dynamicRegistration;
};

/**
 * @since 3.16.0
 */
struct SemanticTokensWorkspaceClientCapabilities {
  // Whether the client implementation supports a refresh request sent from
  // the server to the client.
  //
  // Note that this event is global and will force the client to refresh all
  // semantic tokens currently shown. It should be used with absolute care
  // and is useful for situation where a server for example detects a project
  // wide change that requires such a calculation.
  std::optional<bool> refreshSupport;
};

/**
 * @since 3.16.0
 */
struct CodeLensWorkspaceClientCapabilities {
  // Whether the client implementation supports a refresh request sent from the
  // server to the client.
  //
  // Note that this event is global and will force the client to refresh all
  // code lenses currently shown. It should be used with absolute care and is
  // useful for situation where a server for example detect a project wide
  // change that requires such a calculation.
  std::optional<bool> refreshSupport;
};

/**
 * Capabilities relating to events from file operations by the user in the client.
 *
 * These events do not come from the file system, they come from user operations
 * like renaming a file in the UI.
 *
 * @since 3.16.0
 */
struct FileOperationClientCapabilities {
  // Whether the client supports dynamic registration for file requests/notifications.
  std::optional<bool> dynamicRegistration;

  // The client has support for sending didCreateFiles notifications.
  std::optional<bool> didCreate;

  // The client has support for sending willCreateFiles requests.
  std::optional<bool> willCreate;

  // The client has support for sending didRenameFiles notifications.
  std::optional<bool> didRename;

  // The client has support for sending willRenameFiles requests.
  std::optional<bool> willRename;

  // The client has support for sending didDeleteFiles notifications.
  std::optional<bool> didDelete;

  // The client has support for sending willDeleteFiles requests.
  std::optional<bool> willDelete;
};

/**
 * Client workspace capabilities specific to inline values.
 *
 * @since 3.17.0
 */
struct InlineValueWorkspaceClientCapabilities {
  // Whether the client implementation supports a refresh request sent from the
  // server to the client.
  //
  // Note that this event is global and will force the client to refresh all
  // inline values currently shown. It should be used with absolute care and is
  // useful for situation where a server for example detects a project wide
  // change that requires such a calculation.
  std::optional<bool> refreshSupport;
};

/**
 * Client workspace capabilities specific to inlay hints.
 *
 * @since 3.17.0
 */
struct InlayHintWorkspaceClientCapabilities {
  // Whether the client implementation supports a refresh request sent from
  // the server to the client.
  //
  // Note that this event is global and will force the client to refresh all
  // inlay hints currently shown. It should be used with absolute care and
  // is useful for situation where a server for example detects a project wide
  // change that requires such a calculation.
  std::optional<bool> refreshSupport;
};

/**
 * Workspace client capabilities specific to diagnostic pull requests.
 *
 * @since 3.17.0
 */
struct DiagnosticWorkspaceClientCapabilities {
  // Whether the client implementation supports a refresh request sent from
  // the server to the client.
  //
  // Note that this event is global and will force the client to refresh all
  // pulled diagnostics currently shown. It should be used with absolute care and
  // is useful for situation where a server for example detects a project wide
  // change that requires such a calculation.
  std::optional<bool> refreshSupport;
};

/**
 * Client workspace capabilities specific to folding ranges
 *
 * @since 3.18.0
 * @proposed
 */
struct FoldingRangeWorkspaceClientCapabilities {
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
  std::optional<bool> refreshSupport;
};

/**
 * Client capabilities for a text document content provider.
 *
 * @since 3.18.0
 * @proposed
 */
struct TextDocumentContentClientCapabilities {
  // Text document content provider supports dynamic registration.
  std::optional<bool> dynamicRegistration;
};

struct TextDocumentSyncClientCapabilities {
  // Whether text document synchronization supports dynamic registration.
  std::optional<bool> dynamicRegistration;

  // The client supports sending will save notifications.
  std::optional<bool> willSave;

  // The client supports sending a will save request and
  // waits for a response providing text edits which will
  // be applied to the document before it is saved.
  std::optional<bool> willSaveWaitUntil;

  // The client supports did save notifications.
  std::optional<bool> didSave;
};

struct TextDocumentFilterClientCapabilities {
  // The client supports Relative Patterns.
  //
  // @since 3.18.0
  std::optional<bool> relativePatternSupport;
};

/**
 * @since 3.14.0
 */
struct DeclarationClientCapabilities {
  // Whether declaration supports dynamic registration. If this is set to `true`
  // the client supports the new `DeclarationRegistrationOptions` return value
  // for the corresponding server capability as well.
  std::optional<bool> dynamicRegistration;

  // The client supports additional metadata in the form of declaration links.
  std::optional<bool> linkSupport;
};

/**
 * Client Capabilities for a {@link DefinitionRequest}.
 */
struct DefinitionClientCapabilities {
  // Whether definition supports dynamic registration.
  std::optional<bool> dynamicRegistration;

  // The client supports additional metadata in the form of definition links.
  //
  // @since 3.14.0
  std::optional<bool> linkSupport;
};

/**
 * Since 3.6.0
 */
struct TypeDefinitionClientCapabilities {
  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `TypeDefinitionRegistrationOptions` return value
  // for the corresponding server capability as well.
  std::optional<bool> dynamicRegistration;

  // The client supports additional metadata in the form of definition links.
  //
  // Since 3.14.0
  std::optional<bool> linkSupport;
};

/**
 * @since 3.6.0
 */
struct ImplementationClientCapabilities {
  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `ImplementationRegistrationOptions` return value
  // for the corresponding server capability as well.
  std::optional<bool> dynamicRegistration;

  // The client supports additional metadata in the form of definition links.
  //
  // @since 3.14.0
  std::optional<bool> linkSupport;
};

/**
 * Client Capabilities for a {@link ReferencesRequest}.
 */
struct ReferenceClientCapabilities {
  // Whether references supports dynamic registration.
  std::optional<bool> dynamicRegistration;
};

/**
 * Client Capabilities for a {@link DocumentHighlightRequest}.
 */
struct DocumentHighlightClientCapabilities {
  // Whether document highlight supports dynamic registration.
  std::optional<bool> dynamicRegistration;
};

/**
 * The client capabilities of a {@link DocumentLinkRequest}.
 */
struct DocumentLinkClientCapabilities {
  // Whether document link supports dynamic registration.
  std::optional<bool> dynamicRegistration;

  // Whether the client supports the `tooltip` property on `DocumentLink`.
  //
  // @since 3.15.0
  std::optional<bool> tooltipSupport;
};

struct DocumentColorClientCapabilities {
  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `DocumentColorRegistrationOptions` return value
  // for the corresponding server capability as well.
  std::optional<bool> dynamicRegistration;
};

/**
 * Client capabilities of a {@link DocumentFormattingRequest}.
 */
struct DocumentFormattingClientCapabilities {
  // Whether formatting supports dynamic registration.
  std::optional<bool> dynamicRegistration;
};

/**
 * Client capabilities of a {@link DocumentRangeFormattingRequest}.
 */
struct DocumentRangeFormattingClientCapabilities {
  // Whether range formatting supports dynamic registration.
  std::optional<bool> dynamicRegistration;

  // Whether the client supports formatting multiple ranges at once.
  //
  // @since 3.18.0
  // @proposed
  std::optional<bool> rangesSupport;
};

/**
 * Client capabilities of a {@link DocumentOnTypeFormattingRequest}.
 */
struct DocumentOnTypeFormattingClientCapabilities {
  // Whether on type formatting supports dynamic registration.
  std::optional<bool> dynamicRegistration;
};

struct SelectionRangeClientCapabilities {
  // Whether implementation supports dynamic registration for selection range providers. If this is set to `true`
  // the client supports the new `SelectionRangeRegistrationOptions` return value for the corresponding server
  // capability as well.
  std::optional<bool> dynamicRegistration;
};

/**
 * @since 3.16.0
 */
struct CallHierarchyClientCapabilities {
  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)`
  // return value for the corresponding server capability as well.
  std::optional<bool> dynamicRegistration;
};

/**
 * Client capabilities for the linked editing range request.
 *
 * @since 3.16.0
 */
struct LinkedEditingRangeClientCapabilities {
  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)`
  // return value for the corresponding server capability as well.
  std::optional<bool> dynamicRegistration;
};

/**
 * Client capabilities specific to the moniker request.
 *
 * @since 3.16.0
 */
struct MonikerClientCapabilities {
  // Whether moniker supports dynamic registration. If this is set to `true`
  // the client supports the new `MonikerRegistrationOptions` return value
  // for the corresponding server capability as well.
  std::optional<bool> dynamicRegistration;
};

/**
 * @since 3.17.0
 */
struct TypeHierarchyClientCapabilities {
  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)`
  // return value for the corresponding server capability as well.
  std::optional<bool> dynamicRegistration;
};

/**
 * Client capabilities specific to inline values.
 *
 * @since 3.17.0
 */
struct InlineValueClientCapabilities {
  // Whether implementation supports dynamic registration for inline value providers.
  std::optional<bool> dynamicRegistration;
};

/**
 * Client capabilities specific to inline completions.
 *
 * @since 3.18.0
 * @proposed
 */
struct InlineCompletionClientCapabilities {
  // Whether implementation supports dynamic registration for inline completion providers.
  std::optional<bool> dynamicRegistration;
};

/**
 * Notebook specific client capabilities.
 *
 * @since 3.17.0
 */
struct NotebookDocumentSyncClientCapabilities {
  // Whether implementation supports dynamic registration. If this is
  // set to `true` the client supports the new
  // `(TextDocumentRegistrationOptions & StaticRegistrationOptions)`
  // return value for the corresponding server capability as well.
  std::optional<bool> dynamicRegistration;

  // The client supports sending execution summary data per cell.
  std::optional<bool> executionSummarySupport;
};

/**
 * Client capabilities for the showDocument request.
 *
 * @since 3.16.0
 */
struct ShowDocumentClientCapabilities {
  // The client has support for the showDocument
  // request.
  bool support;
};

/**
 * @since 3.18.0
 */
struct StaleRequestSupportOptions {
  // The client will actively cancel the request.
  bool cancel;

  // The list of requests for which the client
  // will retry the request if it receives a
  // response with error code `ContentModified`
  std::vector<std::string_view> retryOnContentModified;
};

/**
 * Client capabilities specific to the used markdown parser.
 *
 * @since 3.16.0
 */
struct MarkdownClientCapabilities {
  // The name of the parser.
  std::string_view parser;

  // The version of the parser.
  std::optional<std::string_view> version;

  // A list of HTML tags that the client allows / supports in
  // Markdown.
  //
  // @since 3.17.0
  std::optional<std::vector<std::string_view>> allowedTags;
};

/**
 * @since 3.18.0
 */
struct ChangeAnnotationsSupportOptions {
  // Whether the client groups edits with equal labels into tree nodes,
  // for instance all edits labelled with "Changes in Strings" would
  // be a tree node.
  std::optional<bool> groupsOnLabel;
};

/**
 * @since 3.18.0
 */
struct ClientSymbolResolveOptions {
  // The properties that a client can resolve lazily. Usually
  // `location.range`
  std::vector<std::string_view> properties;
};

/**
 * The client supports the following `CompletionList` specific
 * capabilities.
 *
 * @since 3.17.0
 */
struct CompletionListCapabilities {
  // The client supports the following itemDefaults on
  // a completion list.
  //
  // The value lists the supported property names of the
  // `CompletionList.itemDefaults` object. If omitted
  // no properties are supported.
  //
  // @since 3.17.0
  std::optional<std::vector<std::string_view>> itemDefaults;

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
  std::optional<bool> applyKindSupport;
};

/**
 * @since 3.18.0
 */
struct ClientCodeActionResolveOptions {
  // The properties that a client can resolve lazily.
  std::vector<std::string_view> properties;
};

/**
 * @since 3.18.0
 */
struct ClientCodeLensResolveOptions {
  // The properties that a client can resolve lazily.
  std::vector<std::string_view> properties;
};

/**
 * @since 3.18.0
 */
struct ClientFoldingRangeOptions {
  // If set, the client signals that it supports setting collapsedText on
  // folding ranges to display custom labels instead of the default text.
  //
  // @since 3.17.0
  std::optional<bool> collapsedText;
};

/**
 * @since 3.18.0
 */
struct ClientInlayHintResolveOptions {
  // The properties that a client can resolve lazily.
  std::vector<std::string_view> properties;
};

/**
 * @since 3.18.0
 */
struct ClientShowMessageActionItemOptions {
  // Whether the client supports additional attributes which
  // are preserved and send back to the server in the
  // request's response.
  std::optional<bool> additionalPropertiesSupport;
};

/**
 * @since 3.18.0
 */
struct ClientCompletionItemResolveOptions {
  // The properties that a client can resolve lazily.
  std::vector<std::string_view> properties;
};

/**
 * @since 3.18.0
 */
struct ClientSignatureParameterInformationOptions {
  // The client supports processing label offsets instead of a
  // simple label string.
  //
  // @since 3.14.0
  std::optional<bool> labelOffsetSupport;
};

/**
 * @since 3.18.0
 */
struct ClientSemanticTokensRequestFullDelta {
  // The client will send the `textDocument/semanticTokens/full/delta` request if
  // the server provides a corresponding handler.
  std::optional<bool> delta;
};

/**
 * Represents a folding range. To be valid, start and end line must be bigger than zero and smaller
 * than the number of lines in the document. Clients are free to ignore invalid ranges.
 */
struct FoldingRange {
  // The zero-based start line of the range to fold. The folded area starts after the line's last character.
  // To be valid, the end must be zero or larger and smaller than the number of lines in the document.
  std::uint32_t startLine;

  // The zero-based character offset from where the folded range starts. If not defined, defaults to the length of the
  // start line.
  std::optional<std::uint32_t> startCharacter;

  // The zero-based end line of the range to fold. The folded area ends with the line's last character.
  // To be valid, the end must be zero or larger and smaller than the number of lines in the document.
  std::uint32_t endLine;

  // The zero-based character offset before the folded range ends. If not defined, defaults to the length of the end
  // line.
  std::optional<std::uint32_t> endCharacter;

  // Describes the kind of the folding range such as 'comment' or 'region'. The kind
  // is used to categorize folding ranges and used by commands like 'Fold all comments'.
  // See {@link FoldingRangeKind} for an enumeration of standardized kinds.
  std::optional<FoldingRangeKind> kind;

  // The text that the client should show when the specified range is
  // collapsed. If not defined or not supported by the client, a default
  // will be chosen by the client.
  //
  // @since 3.17.0
  std::optional<std::string_view> collapsedText;
};

/**
 * @since 3.18.0
 */
struct ClientFoldingRangeKindOptions {
  // The folding range kind values the client supports. When this
  // property exists the client also guarantees that it will
  // handle values outside its set gracefully and falls back
  // to a default value when unknown.
  std::optional<std::vector<FoldingRangeKind>> valueSet;
};

/**
 * @since 3.18.0
 */
struct ClientSymbolKindOptions {
  // The symbol kind values the client supports. When this
  // property exists the client also guarantees that it will
  // handle values outside its set gracefully and falls back
  // to a default value when unknown.
  //
  // If this property is not present the client only supports
  // the symbol kinds from `File` to `Array` as defined in
  // the initial version of the protocol.
  std::optional<std::vector<SymbolKind>> valueSet;
};

/**
 * A base for all symbol information.
 */
struct BaseSymbolInformation {
  // The name of this symbol.
  std::string_view name;

  // The kind of this symbol.
  SymbolKind kind;

  // Tags for this symbol.
  //
  // @since 3.16.0
  std::optional<std::vector<SymbolTag>> tags;

  // The name of the symbol containing this symbol. This information is for
  // user interface purposes (e.g. to render a qualifier in the user interface
  // if necessary). It can't be used to re-infer a hierarchy for the document
  // symbols.
  std::optional<std::string_view> containerName;
};

/**
 * @since 3.18.0
 */
struct ClientSymbolTagOptions {
  // The tags supported by the client.
  std::vector<SymbolTag> valueSet;
};

/**
 * Moniker definition to match LSIF 0.5 moniker definition.
 *
 * @since 3.16.0
 */
struct Moniker {
  // The scheme of the moniker. For example tsc or .Net
  std::string_view scheme;

  // The identifier of the moniker. The value is opaque in LSIF however
  // schema owners are allowed to define the structure if they want.
  std::string_view identifier;

  // The scope in which the moniker is unique
  UniquenessLevel unique;

  // The moniker kind if known.
  std::optional<MonikerKind> kind;
};

/**
 * The parameters of a notification message.
 */
struct ShowMessageParams {
  // The message type. See {@link MessageType}
  MessageType type;

  // The actual message.
  std::string_view message;
};

/**
 * The log message parameters.
 */
struct LogMessageParams {
  // The message type. See {@link MessageType}
  MessageType type;

  // The actual message.
  std::string_view message;
};

/**
 * @since 3.18.0
 */
struct ClientCompletionItemOptionsKind {
  // The completion item kind values the client supports. When this
  // property exists the client also guarantees that it will
  // handle values outside its set gracefully and falls back
  // to a default value when unknown.
  //
  // If this property is not present the client only supports
  // the completion items kinds from `Text` to `Reference` as defined in
  // the initial version of the protocol.
  std::optional<std::vector<CompletionItemKind>> valueSet;
};

/**
 * @since 3.18.0
 */
struct CompletionItemTagOptions {
  // The tags supported by the client.
  std::vector<CompletionItemTag> valueSet;
};

/**
 * @since 3.18.0
 */
struct ClientCompletionItemInsertTextModeOptions {
  std::vector<InsertTextMode> valueSet;
};

/**
 * @since 3.18.0
 */
struct ClientCodeActionKindOptions {
  // The code action kind values the client supports. When this
  // property exists the client also guarantees that it will
  // handle values outside its set gracefully and falls back
  // to a default value when unknown.
  std::vector<CodeActionKind> valueSet;
};

/**
 * @since 3.18.0 - proposed
 */
struct CodeActionTagOptions {
  // The tags supported by the client.
  std::vector<CodeActionTag> valueSet;
};

struct SetTraceParams {
  TraceValue value;
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
struct MarkupContent {
  // The type of the Markup
  MarkupKind kind;

  // The content itself
  std::string_view value;
};

struct HoverClientCapabilities {
  // Whether hover supports dynamic registration.
  std::optional<bool> dynamicRegistration;

  // Client supports the following content formats for the content
  // property. The order describes the preferred format of the client.
  std::optional<std::vector<MarkupKind>> contentFormat;
};

/**
 * An item to transfer a text document from the client to the
 * server.
 */
struct TextDocumentItem {
  // The text document's uri.
  std::string_view uri;

  // The text document's language identifier.
  LanguageKind languageId;

  // The version number of this document (it will increase after each
  // change, including undo/redo).
  std::int32_t version;

  // The content of the opened text document.
  std::string text;
};

/**
 * An event describing a file change.
 */
struct FileEvent {
  // The file's uri.
  std::string_view uri;

  // The change type.
  FileChangeType type;
};

/**
 * @since 3.18.0
 */
struct ClientDiagnosticsTagOptions {
  // The tags supported by the client.
  std::vector<DiagnosticTag> valueSet;
};

/**
 * Contains additional information about the context in which a completion request is triggered.
 */
struct CompletionContext {
  // How the completion was triggered.
  CompletionTriggerKind triggerKind;

  // The trigger character (a single character) that has trigger code complete.
  // Is undefined if `triggerKind !== CompletionTriggerKind.TriggerCharacter`
  std::optional<std::string_view> triggerCharacter;
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
struct CompletionItemApplyKinds {
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
  std::optional<ApplyKind> commitCharacters;

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
  std::optional<ApplyKind> data;
};

struct RenameClientCapabilities {
  // Whether rename supports dynamic registration.
  std::optional<bool> dynamicRegistration;

  // Client supports testing for validity of rename operations
  // before execution.
  //
  // @since 3.12.0
  std::optional<bool> prepareSupport;

  // Client supports the default behavior result.
  //
  // The value indicates the default behavior used by the
  // client.
  //
  // @since 3.16.0
  std::optional<PrepareSupportDefaultBehavior> prepareSupportDefaultBehavior;

  // Whether the client honors the change annotations in
  // text edits and resource operations returned via the
  // rename request's workspace edit by for example presenting
  // the workspace edit in the user interface and asking
  // for confirmation.
  //
  // @since 3.16.0
  std::optional<bool> honorsChangeAnnotations;
};

struct WorkDoneProgressCreateParams {
  // The token to be used to report progress.
  ProgressToken token;
};

struct WorkDoneProgressCancelParams {
  // The token to be used to report progress.
  ProgressToken token;
};

struct WorkDoneProgressParams {
  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
};

struct PartialResultParams {
  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * A generic resource operation.
 */
struct ResourceOperation {
  // The resource operation kind.
  std::string_view kind;

  // An optional annotation identifier describing the operation.
  //
  // @since 3.16.0
  std::optional<ChangeAnnotationIdentifier> annotationId;
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
using MarkedString = std::variant<std::string_view, MarkedStringWithLanguage>;

/**
 * The parameters of a change configuration notification.
 */
struct DidChangeConfigurationParams {
  // The actual changed settings
  LSPAny settings;
};

/**
 * Represents a reference to a command. Provides a title which
 * will be used to represent a command in the UI and, optionally,
 * an array of arguments which will be passed to the command handler
 * function when invoked.
 */
struct Command {
  // Title of the command, like `save`.
  std::string_view title;

  // An optional tooltip.
  //
  // @since 3.18.0
  // @proposed
  std::optional<std::string_view> tooltip;

  // The identifier of the actual command handler.
  std::string_view command;

  // Arguments that the command handler should be
  // invoked with.
  std::optional<std::vector<LSPAny>> arguments;
};

struct ProgressParams {
  // The progress token provided by the client or server.
  ProgressToken token;

  // The progress data.
  LSPAny value;
};

/**
 * General parameters to register for a notification or to register a provider.
 */
struct Registration {
  // The id used to register the request. The id can be used to deregister
  // the request again.
  std::string_view id;

  // The method / capability to register for.
  std::string_view method;

  // Options necessary for the registration.
  std::optional<LSPAny> registerOptions;
};

/**
 * Client capabilities specific to regular expressions.
 *
 * @since 3.16.0
 */
struct RegularExpressionsClientCapabilities {
  // The engine's name.
  RegularExpressionEngineKind engine;

  // The engine's version.
  std::optional<std::string_view> version;
};

/**
 * The workspace folder change event.
 */
struct WorkspaceFoldersChangeEvent {
  // The array of added workspace folders
  std::vector<WorkspaceFolder> added;

  // The array of the removed workspace folders
  std::vector<WorkspaceFolder> removed;
};

struct WorkspaceFoldersInitializeParams {
  // The workspace folders configured in the client when the server starts.
  //
  // This property is only available if the client supports workspace folders.
  // It can be `null` if the client supports workspace folders but none are
  // configured.
  //
  // @since 3.6.0
  std::optional<std::vector<WorkspaceFolder>> workspaceFolders;
};

/**
 * A relative pattern is a helper to construct glob patterns that are matched
 * relatively to a base URI. The common value for a `baseUri` is a workspace
 * folder root, but it can be another absolute URI as well.
 *
 * @since 3.17.0
 */
struct RelativePattern {
  // A workspace folder or a base URI to which this pattern will be matched
  // against relatively.
  std::variant<WorkspaceFolder, std::string_view> baseUri;

  // The actual glob pattern;
  Pattern pattern;
};

/**
 * The parameters of a configuration request.
 */
struct ConfigurationParams {
  std::vector<ConfigurationItem> items;
};

/**
 * The parameters sent in a close text document notification
 */
struct DidCloseTextDocumentParams {
  // The document that was closed.
  TextDocumentIdentifier textDocument;
};

/**
 * The parameters sent in a save text document notification
 */
struct DidSaveTextDocumentParams {
  // The document that was saved.
  TextDocumentIdentifier textDocument;

  // Optional the content when saved. Depends on the includeText value
  // when the save notification was requested.
  std::optional<std::string_view> text;
};

/**
 * The parameters sent in a will save text document notification.
 */
struct WillSaveTextDocumentParams {
  // The document that will be saved.
  TextDocumentIdentifier textDocument;

  // The 'TextDocumentSaveReason'.
  TextDocumentSaveReason reason;
};

/**
 * A text document identifier to denote a specific version of a text document.
 */
struct VersionedTextDocumentIdentifier {
  // The version number of this document.
  std::int32_t version;

  // The text document's uri.
  std::string_view uri;
};

/**
 * A text document identifier to optionally denote a specific version of a text document.
 */
struct OptionalVersionedTextDocumentIdentifier {
  // The version number of this document. If a versioned text document identifier
  // is sent from the server to the client and the file is not open in the editor
  // (the server has not received an open notification before) the server can send
  // `null` to indicate that the version is unknown and the content on disk is the
  // truth (as specified with document content ownership).
  std::int32_t version;

  // The text document's uri.
  std::string_view uri;
};

struct ImplementationOptions {
  std::optional<bool> workDoneProgress;
};

struct TypeDefinitionOptions {
  std::optional<bool> workDoneProgress;
};

struct DocumentColorOptions {
  std::optional<bool> workDoneProgress;
};

struct FoldingRangeOptions {
  std::optional<bool> workDoneProgress;
};

struct DeclarationOptions {
  std::optional<bool> workDoneProgress;
};

struct SelectionRangeOptions {
  std::optional<bool> workDoneProgress;
};

/**
 * Call hierarchy options used during static registration.
 *
 * @since 3.16.0
 */
struct CallHierarchyOptions {
  std::optional<bool> workDoneProgress;
};

struct LinkedEditingRangeOptions {
  std::optional<bool> workDoneProgress;
};

struct MonikerOptions {
  std::optional<bool> workDoneProgress;
};

/**
 * Type hierarchy options used during static registration.
 *
 * @since 3.17.0
 */
struct TypeHierarchyOptions {
  std::optional<bool> workDoneProgress;
};

/**
 * Inline value options used during static registration.
 *
 * @since 3.17.0
 */
struct InlineValueOptions {
  std::optional<bool> workDoneProgress;
};

/**
 * Inlay hint options used during static registration.
 *
 * @since 3.17.0
 */
struct InlayHintOptions {
  // The server provides support to resolve additional
  // information for an inlay hint item.
  std::optional<bool> resolveProvider;
  std::optional<bool> workDoneProgress;
};

/**
 * Diagnostic options.
 *
 * @since 3.17.0
 */
struct DiagnosticOptions {
  // An optional identifier under which the diagnostics are
  // managed by the client.
  std::optional<std::string_view> identifier;

  // Whether the language has inter file dependencies meaning that
  // editing code in one file can result in a different diagnostic
  // set in another file. Inter file dependencies are common for
  // most programming languages and typically uncommon for linters.
  bool interFileDependencies;

  // The server provides support for workspace diagnostics as well.
  bool workspaceDiagnostics;
  std::optional<bool> workDoneProgress;
};

/**
 * Inline completion options used during static registration.
 *
 * @since 3.18.0
 * @proposed
 */
struct InlineCompletionOptions {
  std::optional<bool> workDoneProgress;
};

/**
 * Hover options.
 */
struct HoverOptions {
  std::optional<bool> workDoneProgress;
};

/**
 * Server Capabilities for a {@link SignatureHelpRequest}.
 */
struct SignatureHelpOptions {
  // List of characters that trigger signature help automatically.
  std::optional<std::vector<std::string_view>> triggerCharacters;

  // List of characters that re-trigger signature help.
  //
  // These trigger characters are only active when signature help is already showing. All trigger characters
  // are also counted as re-trigger characters.
  //
  // @since 3.15.0
  std::optional<std::vector<std::string_view>> retriggerCharacters;
  std::optional<bool> workDoneProgress;
};

/**
 * Server Capabilities for a {@link DefinitionRequest}.
 */
struct DefinitionOptions {
  std::optional<bool> workDoneProgress;
};

/**
 * Reference options.
 */
struct ReferenceOptions {
  std::optional<bool> workDoneProgress;
};

/**
 * Provider options for a {@link DocumentHighlightRequest}.
 */
struct DocumentHighlightOptions {
  std::optional<bool> workDoneProgress;
};

/**
 * Provider options for a {@link DocumentSymbolRequest}.
 */
struct DocumentSymbolOptions {
  // A human-readable string that is shown when multiple outlines trees
  // are shown for the same document.
  //
  // @since 3.16.0
  std::optional<std::string_view> label;
  std::optional<bool> workDoneProgress;
};

/**
 * Server capabilities for a {@link WorkspaceSymbolRequest}.
 */
struct WorkspaceSymbolOptions {
  // The server provides support to resolve additional
  // information for a workspace symbol.
  //
  // @since 3.17.0
  std::optional<bool> resolveProvider;
  std::optional<bool> workDoneProgress;
};

/**
 * Code Lens provider options of a {@link CodeLensRequest}.
 */
struct CodeLensOptions {
  // Code lens has a resolve provider as well.
  std::optional<bool> resolveProvider;
  std::optional<bool> workDoneProgress;
};

/**
 * Provider options for a {@link DocumentLinkRequest}.
 */
struct DocumentLinkOptions {
  // Document links have a resolve provider as well.
  std::optional<bool> resolveProvider;
  std::optional<bool> workDoneProgress;
};

/**
 * Provider options for a {@link DocumentFormattingRequest}.
 */
struct DocumentFormattingOptions {
  std::optional<bool> workDoneProgress;
};

/**
 * Provider options for a {@link DocumentRangeFormattingRequest}.
 */
struct DocumentRangeFormattingOptions {
  // Whether the server supports formatting multiple ranges at once.
  //
  // @since 3.18.0
  // @proposed
  std::optional<bool> rangesSupport;
  std::optional<bool> workDoneProgress;
};

/**
 * Provider options for a {@link RenameRequest}.
 */
struct RenameOptions {
  // Renames should be checked and tested before being executed.
  //
  // @since version 3.12.0
  std::optional<bool> prepareProvider;
  std::optional<bool> workDoneProgress;
};

/**
 * The server capabilities of a {@link ExecuteCommandRequest}.
 */
struct ExecuteCommandOptions {
  // The commands to be executed on the server
  std::vector<std::string_view> commands;
  std::optional<bool> workDoneProgress;
};

/**
 * A parameter literal used in requests to pass a text document and a position inside that
 * document.
 */
struct TextDocumentPositionParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;
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
struct Range {
  // The range's start position.
  Position start;

  // The range's end position.
  Position end;
};

/**
 * @since 3.16.0
 */
struct SemanticTokensDelta {
  std::optional<std::string_view> resultId;

  // The semantic token edits to transform a previous result into a new result.
  std::vector<SemanticTokensEdit> edits;
};

/**
 * @since 3.16.0
 */
struct SemanticTokensDeltaPartialResult {
  std::vector<SemanticTokensEdit> edits;
};

/**
 * The parameters sent in notifications/requests for user-initiated creation of
 * files.
 *
 * @since 3.16.0
 */
struct CreateFilesParams {
  // An array of all files/folders created in this operation.
  std::vector<FileCreate> files;
};

/**
 * The parameters sent in notifications/requests for user-initiated renames of
 * files.
 *
 * @since 3.16.0
 */
struct RenameFilesParams {
  // An array of all files/folders renamed in this operation. When a folder is renamed, only
  // the folder will be included, and not its children.
  std::vector<FileRename> files;
};

/**
 * The parameters sent in notifications/requests for user-initiated deletes of
 * files.
 *
 * @since 3.16.0
 */
struct DeleteFilesParams {
  // An array of all files/folders deleted in this operation.
  std::vector<FileDelete> files;
};

/**
 * An unchanged document diagnostic report for a workspace diagnostic result.
 *
 * @since 3.17.0
 */
struct WorkspaceUnchangedDocumentDiagnosticReport {
  // The URI for which diagnostic information is reported.
  std::string_view uri;

  // The version number for which the diagnostics are reported.
  // If the document is not marked as open `null` can be provided.
  std::int32_t version;

  // A document diagnostic report indicating
  // no changes to the last result. A server can
  // only return `unchanged` if result ids are
  // provided.
  const std::string_view kind;

  // A result id which will be sent on the next
  // diagnostic request for the same document.
  std::string_view resultId;
};

/**
 * The params sent in a save notebook document notification.
 *
 * @since 3.17.0
 */
struct DidSaveNotebookDocumentParams {
  // The notebook document that got saved.
  NotebookDocumentIdentifier notebookDocument;
};

/**
 * The params sent in a close notebook document notification.
 *
 * @since 3.17.0
 */
struct DidCloseNotebookDocumentParams {
  // The notebook document that got closed.
  NotebookDocumentIdentifier notebookDocument;

  // The text documents that represent the content
  // of a notebook cell that got closed.
  std::vector<TextDocumentIdentifier> cellTextDocuments;
};

/**
 * Text document content provider registration options.
 *
 * @since 3.18.0
 * @proposed
 */
struct TextDocumentContentRegistrationOptions {
  // The schemes for which the server provides content.
  std::vector<std::string_view> schemes;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

struct UnregistrationParams {
  std::vector<Unregistration> unregisterations;
};

struct ShowMessageRequestParams {
  // The message type. See {@link MessageType}
  MessageType type;

  // The actual message.
  std::string_view message;

  // The message action items to present.
  std::optional<std::vector<MessageActionItem>> actions;
};

struct TextDocumentSyncOptions {
  // Open and close notifications are sent to the server. If omitted open close notification should not
  // be sent.
  std::optional<bool> openClose;

  // Change notifications are sent to the server. See TextDocumentSyncKind.None, TextDocumentSyncKind.Full
  // and TextDocumentSyncKind.Incremental. If omitted it defaults to TextDocumentSyncKind.None.
  std::optional<TextDocumentSyncKind> change;

  // If present will save notifications are sent to the server. If omitted the notification should not be
  // sent.
  std::optional<bool> willSave;

  // If present will save wait until requests are sent to the server. If omitted the request should not be
  // sent.
  std::optional<bool> willSaveWaitUntil;

  // If present save notifications are sent to the server. If omitted the notification should not be
  // sent.
  std::optional<std::variant<bool, SaveOptions>> save;
};

/**
 * The parameters of a {@link DocumentOnTypeFormattingRequest}.
 */
struct DocumentOnTypeFormattingParams {
  // The document to format.
  TextDocumentIdentifier textDocument;

  // The position around which the on type formatting should happen.
  // This is not necessarily the exact position where the character denoted
  // by the property `ch` got typed.
  Position position;

  // The character that has been typed that triggered the formatting
  // on type request. That is not necessarily the last character that
  // got inserted into the document since the client could auto insert
  // characters as well (e.g. like automatic brace completion).
  std::string_view ch;

  // The formatting options.
  FormattingOptions options;
};

/**
 * @since 3.16.0
 */
struct SemanticTokensOptions {
  // The legend used by the server
  SemanticTokensLegend legend;

  // Server supports providing semantic tokens for a specific range
  // of a document.
  std::optional<std::variant<bool, LiteralStub>> range;

  // Server supports providing semantic tokens for a full document.
  std::optional<std::variant<bool, SemanticTokensFullDelta>> full;
  std::optional<bool> workDoneProgress;
};

/**
 * Completion options.
 */
struct CompletionOptions {
  // Most tools trigger completion request automatically without explicitly requesting
  // it using a keyboard shortcut (e.g. Ctrl+Space). Typically they do so when the user
  // starts to type an identifier. For example if the user types `c` in a JavaScript file
  // code complete will automatically pop up present `console` besides others as a
  // completion item. Characters that make up identifiers don't need to be listed here.
  //
  // If code complete should automatically be trigger on characters not being valid inside
  // an identifier (for example `.` in JavaScript) list them in `triggerCharacters`.
  std::optional<std::vector<std::string_view>> triggerCharacters;

  // The list of all possible characters that commit a completion. This field can be used
  // if clients don't support individual commit characters per completion item. See
  // `ClientCapabilities.textDocument.completion.completionItem.commitCharactersSupport`
  //
  // If a server provides both `allCommitCharacters` and commit characters on an individual
  // completion item the ones on the completion item win.
  //
  // @since 3.2.0
  std::optional<std::vector<std::string_view>> allCommitCharacters;

  // The server provides support to resolve additional
  // information for a completion item.
  std::optional<bool> resolveProvider;

  // The server supports the following `CompletionItem` specific
  // capabilities.
  //
  // @since 3.17.0
  std::optional<ServerCompletionItemOptions> completionItem;
  std::optional<bool> workDoneProgress;
};

/**
 * A pattern to describe in which file operation requests or notifications
 * the server is interested in receiving.
 *
 * @since 3.16.0
 */
struct FileOperationPattern {
  // The glob pattern to match. Glob patterns can have the following syntax:
  // - `*` to match one or more characters in a path segment
  // - `?` to match on one character in a path segment
  // - `**` to match any number of path segments, including none
  // - `{}` to group sub patterns into an OR expression. (e.g. `**​/\*.{ts,js}` matches all TypeScript and JavaScript
  // files)
  // - `[]` to declare a range of characters to match in a path segment (e.g., `example.[0-9]` to match on `example.0`,
  // `example.1`, …)
  // - `[!...]` to negate a range of characters to match in a path segment (e.g., `example.[!0-9]` to match on
  // `example.a`, `example.b`, but not `example.0`)
  std::string_view glob;

  // Whether to match files or folders with this pattern.
  //
  // Matches both if undefined.
  std::optional<FileOperationPatternKind> matches;

  // Additional options used during matching.
  std::optional<FileOperationPatternOptions> options;
};

/**
 * Capabilities specific to the notebook document support.
 *
 * @since 3.17.0
 */
struct NotebookDocumentClientCapabilities {
  // Capabilities specific to notebook document synchronization
  //
  // @since 3.17.0
  NotebookDocumentSyncClientCapabilities synchronization;
};

struct WorkspaceEditClientCapabilities {
  // The client supports versioned document changes in `WorkspaceEdit`s
  std::optional<bool> documentChanges;

  // The resource operations the client supports. Clients should at least
  // support 'create', 'rename' and 'delete' files and folders.
  //
  // @since 3.13.0
  std::optional<std::vector<ResourceOperationKind>> resourceOperations;

  // The failure handling strategy of a client if applying the workspace edit
  // fails.
  //
  // @since 3.13.0
  std::optional<FailureHandlingKind> failureHandling;

  // Whether the client normalizes line endings to the client specific
  // setting.
  // If set to `true` the client will normalize line ending characters
  // in a workspace edit to the client-specified new line
  // character.
  //
  // @since 3.16.0
  std::optional<bool> normalizesLineEndings;

  // Whether the client in general supports change annotations on text edits,
  // create file, rename file and delete file changes.
  //
  // @since 3.16.0
  std::optional<ChangeAnnotationsSupportOptions> changeAnnotationSupport;

  // Whether the client supports `WorkspaceEditMetadata` in `WorkspaceEdit`s.
  //
  // @since 3.18.0
  // @proposed
  std::optional<bool> metadataSupport;

  // Whether the client supports snippets as text edits.
  //
  // @since 3.18.0
  // @proposed
  std::optional<bool> snippetEditSupport;
};

/**
 * The client capabilities  of a {@link CodeLensRequest}.
 */
struct CodeLensClientCapabilities {
  // Whether code lens supports dynamic registration.
  std::optional<bool> dynamicRegistration;

  // Whether the client supports resolving additional code lens
  // properties via a separate `codeLens/resolve` request.
  //
  // @since 3.18.0
  std::optional<ClientCodeLensResolveOptions> resolveSupport;
};

/**
 * Inlay hint client capabilities.
 *
 * @since 3.17.0
 */
struct InlayHintClientCapabilities {
  // Whether inlay hints support dynamic registration.
  std::optional<bool> dynamicRegistration;

  // Indicates which properties a client can resolve lazily on an inlay
  // hint.
  std::optional<ClientInlayHintResolveOptions> resolveSupport;
};

/**
 * Show message request client capabilities
 */
struct ShowMessageRequestClientCapabilities {
  // Capabilities specific to the `MessageActionItem` type.
  std::optional<ClientShowMessageActionItemOptions> messageActionItem;
};

/**
 * @since 3.18.0
 */
struct ClientSignatureInformationOptions {
  // Client supports the following content formats for the documentation
  // property. The order describes the preferred format of the client.
  std::optional<std::vector<MarkupKind>> documentationFormat;

  // Client capabilities specific to parameter information.
  std::optional<ClientSignatureParameterInformationOptions> parameterInformation;

  // The client supports the `activeParameter` property on `SignatureInformation`
  // literal.
  //
  // @since 3.16.0
  std::optional<bool> activeParameterSupport;

  // The client supports the `activeParameter` property on
  // `SignatureHelp`/`SignatureInformation` being set to `null` to
  // indicate that no parameter should be active.
  //
  // @since 3.18.0
  // @proposed
  std::optional<bool> noActiveParameterSupport;
};

/**
 * @since 3.18.0
 */
struct ClientSemanticTokensRequestOptions {
  // The client will send the `textDocument/semanticTokens/range` request if
  // the server provides a corresponding handler.
  std::optional<std::variant<bool, LiteralStub>> range;

  // The client will send the `textDocument/semanticTokens/full` request if
  // the server provides a corresponding handler.
  std::optional<std::variant<bool, ClientSemanticTokensRequestFullDelta>> full;
};

struct FoldingRangeClientCapabilities {
  // Whether implementation supports dynamic registration for folding range
  // providers. If this is set to `true` the client supports the new
  // `FoldingRangeRegistrationOptions` return value for the corresponding
  // server capability as well.
  std::optional<bool> dynamicRegistration;

  // The maximum number of folding ranges that the client prefers to receive
  // per document. The value serves as a hint, servers are free to follow the
  // limit.
  std::optional<std::uint32_t> rangeLimit;

  // If set, the client signals that it only supports folding complete lines.
  // If set, client will ignore specified `startCharacter` and `endCharacter`
  // properties in a FoldingRange.
  std::optional<bool> lineFoldingOnly;

  // Specific options for the folding range kind.
  //
  // @since 3.17.0
  std::optional<ClientFoldingRangeKindOptions> foldingRangeKind;

  // Specific options for the folding range.
  //
  // @since 3.17.0
  std::optional<ClientFoldingRangeOptions> foldingRange;
};

/**
 * Client capabilities for a {@link WorkspaceSymbolRequest}.
 */
struct WorkspaceSymbolClientCapabilities {
  // Symbol request supports dynamic registration.
  std::optional<bool> dynamicRegistration;

  // Specific capabilities for the `SymbolKind` in the `workspace/symbol` request.
  std::optional<ClientSymbolKindOptions> symbolKind;

  // The client supports tags on `SymbolInformation`.
  // Clients supporting tags have to handle unknown tags gracefully.
  //
  // @since 3.16.0
  std::optional<ClientSymbolTagOptions> tagSupport;

  // The client support partial workspace symbols. The client will send the
  // request `workspaceSymbol/resolve` to the server to resolve additional
  // properties.
  //
  // @since 3.17.0
  std::optional<ClientSymbolResolveOptions> resolveSupport;
};

/**
 * Client Capabilities for a {@link DocumentSymbolRequest}.
 */
struct DocumentSymbolClientCapabilities {
  // Whether document symbol supports dynamic registration.
  std::optional<bool> dynamicRegistration;

  // Specific capabilities for the `SymbolKind` in the
  // `textDocument/documentSymbol` request.
  std::optional<ClientSymbolKindOptions> symbolKind;

  // The client supports hierarchical document symbols.
  std::optional<bool> hierarchicalDocumentSymbolSupport;

  // The client supports tags on `SymbolInformation`. Tags are supported on
  // `DocumentSymbol` if `hierarchicalDocumentSymbolSupport` is set to true.
  // Clients supporting tags have to handle unknown tags gracefully.
  //
  // @since 3.16.0
  std::optional<ClientSymbolTagOptions> tagSupport;

  // The client supports an additional label presented in the UI when
  // registering a document symbol provider.
  //
  // @since 3.16.0
  std::optional<bool> labelSupport;
};

/**
 * @since 3.18.0
 */
struct ClientCompletionItemOptions {
  // Client supports snippets as insert text.
  //
  // A snippet can define tab stops and placeholders with `$1`, `$2`
  // and `${3:foo}`. `$0` defines the final tab stop, it defaults to
  // the end of the snippet. Placeholders with equal identifiers are linked,
  // that is typing in one will update others too.
  std::optional<bool> snippetSupport;

  // Client supports commit characters on a completion item.
  std::optional<bool> commitCharactersSupport;

  // Client supports the following content formats for the documentation
  // property. The order describes the preferred format of the client.
  std::optional<std::vector<MarkupKind>> documentationFormat;

  // Client supports the deprecated property on a completion item.
  std::optional<bool> deprecatedSupport;

  // Client supports the preselect property on a completion item.
  std::optional<bool> preselectSupport;

  // Client supports the tag property on a completion item. Clients supporting
  // tags have to handle unknown tags gracefully. Clients especially need to
  // preserve unknown tags when sending a completion item back to the server in
  // a resolve call.
  //
  // @since 3.15.0
  std::optional<CompletionItemTagOptions> tagSupport;

  // Client support insert replace edit to control different behavior if a
  // completion item is inserted in the text or should replace text.
  //
  // @since 3.16.0
  std::optional<bool> insertReplaceSupport;

  // Indicates which properties a client can resolve lazily on a completion
  // item. Before version 3.16.0 only the predefined properties `documentation`
  // and `details` could be resolved lazily.
  //
  // @since 3.16.0
  std::optional<ClientCompletionItemResolveOptions> resolveSupport;

  // The client supports the `insertTextMode` property on
  // a completion item to override the whitespace handling mode
  // as defined by the client (see `insertTextMode`).
  //
  // @since 3.16.0
  std::optional<ClientCompletionItemInsertTextModeOptions> insertTextModeSupport;

  // The client has support for completion item label
  // details (see also `CompletionItemLabelDetails`).
  //
  // @since 3.17.0
  std::optional<bool> labelDetailsSupport;
};

/**
 * @since 3.18.0
 */
struct ClientCodeActionLiteralOptions {
  // The code action kind is support with the following value
  // set.
  ClientCodeActionKindOptions codeActionKind;
};

/**
 * Represents a parameter of a callable-signature. A parameter can
 * have a label and a doc-comment.
 */
struct ParameterInformation {
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
  std::variant<std::string_view, std::tuple<std::uint32_t, std::uint32_t>> label;

  // The human-readable doc-comment of this parameter. Will be shown
  // in the UI but can be omitted.
  std::optional<std::variant<std::string_view, MarkupContent>> documentation;
};

/**
 * The parameters sent in an open text document notification
 */
struct DidOpenTextDocumentParams {
  // The document that was opened.
  TextDocumentItem textDocument;
};

/**
 * The watched files change notification's parameters.
 */
struct DidChangeWatchedFilesParams {
  // The actual file events.
  std::vector<FileEvent> changes;
};

/**
 * General diagnostics capabilities for pull and push model.
 */
struct DiagnosticsCapabilities {
  // Whether the clients accepts diagnostics with related information.
  std::optional<bool> relatedInformation;

  // Client supports the tag property to provide meta data about a diagnostic.
  // Clients supporting tags have to handle unknown tags gracefully.
  //
  // @since 3.15.0
  std::optional<ClientDiagnosticsTagOptions> tagSupport;

  // Client supports a codeDescription property
  //
  // @since 3.16.0
  std::optional<bool> codeDescriptionSupport;

  // Whether code action supports the `data` property which is
  // preserved between a `textDocument/publishDiagnostics` and
  // `textDocument/codeAction` request.
  //
  // @since 3.16.0
  std::optional<bool> dataSupport;
};

/**
 * The parameters of a {@link DocumentFormattingRequest}.
 */
struct DocumentFormattingParams {
  // The document to format.
  TextDocumentIdentifier textDocument;

  // The format options.
  FormattingOptions options;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
};

/**
 * The parameters of a {@link RenameRequest}.
 */
struct RenameParams {
  // The document to rename.
  TextDocumentIdentifier textDocument;

  // The position at which this request was sent.
  Position position;

  // The new name of the symbol. If the given name is not valid the
  // request must return a {@link ResponseError} with an
  // appropriate message set.
  std::string_view newName;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
};

/**
 * The parameters of a {@link ExecuteCommandRequest}.
 */
struct ExecuteCommandParams {
  // The identifier of the actual command handler.
  std::string_view command;

  // Arguments that the command should be invoked with.
  std::optional<std::vector<LSPAny>> arguments;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
};

/**
 * Parameters for a {@link DocumentColorRequest}.
 */
struct DocumentColorParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * Parameters for a {@link FoldingRangeRequest}.
 */
struct FoldingRangeParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * A parameter literal used in selection range requests.
 */
struct SelectionRangeParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The positions inside the text document.
  std::vector<Position> positions;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * @since 3.16.0
 */
struct SemanticTokensParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * @since 3.16.0
 */
struct SemanticTokensDeltaParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The result id of a previous response. The result Id can either point to a full response
  // or a delta response depending on what was received last.
  std::string_view previousResultId;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * Parameters of the document diagnostic request.
 *
 * @since 3.17.0
 */
struct DocumentDiagnosticParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The additional identifier  provided during registration.
  std::optional<std::string_view> identifier;

  // The result id of a previous response if provided.
  std::optional<std::string_view> previousResultId;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * Parameters of the workspace diagnostic request.
 *
 * @since 3.17.0
 */
struct WorkspaceDiagnosticParams {
  // The additional identifier provided during registration.
  std::optional<std::string_view> identifier;

  // The currently known diagnostic reports with their
  // previous result ids.
  std::vector<PreviousResultId> previousResultIds;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * Parameters for a {@link DocumentSymbolRequest}.
 */
struct DocumentSymbolParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * The parameters of a {@link WorkspaceSymbolRequest}.
 */
struct WorkspaceSymbolParams {
  // A query string to filter symbols by. Clients may send an empty
  // string here to request all symbols.
  //
  // The `query`-parameter should be interpreted in a *relaxed way* as editors
  // will apply their own highlighting and scoring on the results. A good rule
  // of thumb is to match case-insensitive and to simply check that the
  // characters of *query* appear in their order in a candidate symbol.
  // Servers shouldn't use prefix, substring, or similar strict matching.
  std::string_view query;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * The parameters of a {@link CodeLensRequest}.
 */
struct CodeLensParams {
  // The document to request code lens for.
  TextDocumentIdentifier textDocument;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * The parameters of a {@link DocumentLinkRequest}.
 */
struct DocumentLinkParams {
  // The document to provide document links for.
  TextDocumentIdentifier textDocument;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * Create file operation.
 */
struct CreateFile {
  // A create
  const std::string_view kind;

  // The resource to create.
  std::string_view uri;

  // Additional options
  std::optional<CreateFileOptions> options;

  // An optional annotation identifier describing the operation.
  //
  // @since 3.16.0
  std::optional<ChangeAnnotationIdentifier> annotationId;
};

/**
 * Rename file operation
 */
struct RenameFile {
  // A rename
  const std::string_view kind;

  // The old (existing) location.
  std::string_view oldUri;

  // The new location.
  std::string_view newUri;

  // Rename options.
  std::optional<RenameFileOptions> options;

  // An optional annotation identifier describing the operation.
  //
  // @since 3.16.0
  std::optional<ChangeAnnotationIdentifier> annotationId;
};

/**
 * Delete file operation
 */
struct DeleteFile {
  // A delete
  const std::string_view kind;

  // The file to delete.
  std::string_view uri;

  // Delete options.
  std::optional<DeleteFileOptions> options;

  // An optional annotation identifier describing the operation.
  //
  // @since 3.16.0
  std::optional<ChangeAnnotationIdentifier> annotationId;
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
struct NotebookCell {
  // The cell's kind
  NotebookCellKind kind;

  // The URI of the cell's text document
  // content.
  std::string_view document;

  // Additional metadata stored with the cell.
  //
  // Note: should always be an object literal (e.g. LSPObject)
  std::optional<LSPObject> metadata;

  // Additional execution summary information
  // if supported by the client.
  std::optional<ExecutionSummary> executionSummary;
};

/**
 * Documentation for a class of code actions.
 *
 * @since 3.18.0
 * @proposed
 */
struct CodeActionKindDocumentation {
  // The kind of the code action being documented.
  //
  // If the kind is generic, such as `CodeActionKind.Refactor`, the documentation will be shown whenever any
  // refactorings are returned. If the kind if more specific, such as `CodeActionKind.RefactorExtract`, the
  // documentation will only be shown when extract refactoring code actions are returned.
  CodeActionKind kind;

  // Command that is ued to display the documentation to the user.
  //
  // The title of this documentation code action is taken from {@linkcode Command.title}
  Command command;
};

struct RegistrationParams {
  std::vector<Registration> registrations;
};

/**
 * General client capabilities.
 *
 * @since 3.16.0
 */
struct GeneralClientCapabilities {
  // Client capability that signals how the client
  // handles stale requests (e.g. a request
  // for which the client will not process the response
  // anymore since the information is outdated).
  //
  // @since 3.17.0
  std::optional<StaleRequestSupportOptions> staleRequestSupport;

  // Client capabilities specific to regular expressions.
  //
  // @since 3.16.0
  std::optional<RegularExpressionsClientCapabilities> regularExpressions;

  // Client capabilities specific to the client's markdown parser.
  //
  // @since 3.16.0
  std::optional<MarkdownClientCapabilities> markdown;

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
  std::optional<std::vector<PositionEncodingKind>> positionEncodings;
};

/**
 * The parameters of a `workspace/didChangeWorkspaceFolders` notification.
 */
struct DidChangeWorkspaceFoldersParams {
  // The actual workspace folder change event.
  WorkspaceFoldersChangeEvent event;
};

/**
 * The glob pattern. Either a string pattern or a relative pattern.
 *
 * @since 3.17.0
 */
using GlobPattern = std::variant<Pattern, RelativePattern>;

/**
 * Registration options for a {@link WorkspaceSymbolRequest}.
 */
struct WorkspaceSymbolRegistrationOptions {
  // The server provides support to resolve additional
  // information for a workspace symbol.
  //
  // @since 3.17.0
  std::optional<bool> resolveProvider;
};

/**
 * Registration options for a {@link ExecuteCommandRequest}.
 */
struct ExecuteCommandRegistrationOptions {
  // The commands to be executed on the server
  std::vector<std::string_view> commands;
};

struct ImplementationParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

struct TypeDefinitionParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

struct DeclarationParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * The parameter of a `textDocument/prepareCallHierarchy` request.
 *
 * @since 3.16.0
 */
struct CallHierarchyPrepareParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
};

struct LinkedEditingRangeParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
};

struct MonikerParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * The parameter of a `textDocument/prepareTypeHierarchy` request.
 *
 * @since 3.17.0
 */
struct TypeHierarchyPrepareParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
};

/**
 * Completion parameters
 */
struct CompletionParams {
  // The completion context. This is only available it the client specifies
  // to send this using the client capability `textDocument.completion.contextSupport === true`
  std::optional<CompletionContext> context;

  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * Parameters for a {@link HoverRequest}.
 */
struct HoverParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
};

/**
 * Parameters for a {@link DefinitionRequest}.
 */
struct DefinitionParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * Parameters for a {@link ReferencesRequest}.
 */
struct ReferenceParams {
  ReferenceContext context;

  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * Parameters for a {@link DocumentHighlightRequest}.
 */
struct DocumentHighlightParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

struct PrepareRenameParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
};

/**
 * Represents a location inside a resource, such as a line
 * inside a text file.
 */
struct Location {
  std::string_view uri;
  Range range;
};

/**
 * Represents a color range from a document.
 */
struct ColorInformation {
  // The range in the document where this color appears.
  Range range;

  // The actual color value for this color range.
  Color color;
};

/**
 * Parameters for a {@link ColorPresentationRequest}.
 */
struct ColorPresentationParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The color to request presentations for.
  Color color;

  // The range where the color would be inserted. Serves as a context.
  Range range;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * A selection range represents a part of a selection hierarchy. A selection range
 * may have a parent selection range that contains it.
 */
struct SelectionRange {
  // The {@link Range range} of this selection range.
  Range range;

  // The parent selection range containing this range. Therefore `parent.range` must contain `this.range`.
  SelectionRange* parent{nullptr};
};

/**
 * Represents programming constructs like functions or constructors in the context
 * of call hierarchy.
 *
 * @since 3.16.0
 */
struct CallHierarchyItem {
  // The name of this item.
  std::string_view name;

  // The kind of this item.
  SymbolKind kind;

  // Tags for this item.
  std::optional<std::vector<SymbolTag>> tags;

  // More detail for this item, e.g. the signature of a function.
  std::optional<std::string_view> detail;

  // The resource identifier of this item.
  std::string_view uri;

  // The range enclosing this symbol not including leading/trailing whitespace but everything else, e.g. comments and
  // code.
  Range range;

  // The range that should be selected and revealed when this symbol is being picked, e.g. the name of a function.
  // Must be contained by the {@link CallHierarchyItem.range `range`}.
  Range selectionRange;

  // A data entry field that is preserved between a call hierarchy prepare and
  // incoming calls or outgoing calls requests.
  std::optional<LSPAny> data;
};

/**
 * @since 3.16.0
 */
struct SemanticTokensRangeParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The range the semantic tokens are requested for.
  Range range;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * Params to show a resource in the UI.
 *
 * @since 3.16.0
 */
struct ShowDocumentParams {
  // The uri to show.
  std::string_view uri;

  // Indicates to show the resource in an external program.
  // To show, for example, `https://code.visualstudio.com/`
  // in the default WEB browser set `external` to `true`.
  std::optional<bool> external;

  // An optional property to indicate whether the editor
  // showing the document should take focus or not.
  // Clients might ignore this property if an external
  // program is started.
  std::optional<bool> takeFocus;

  // An optional selection range if the document is a text
  // document. Clients might ignore the property if an
  // external program is started or the file is not a text
  // file.
  std::optional<Range> selection;
};

/**
 * The result of a linked editing range request.
 *
 * @since 3.16.0
 */
struct LinkedEditingRanges {
  // A list of ranges that can be edited together. The ranges must have
  // identical length and contain identical text content. The ranges cannot overlap.
  std::vector<Range> ranges;

  // An optional word pattern (regular expression) that describes valid contents for
  // the given ranges. If no pattern is provided, the client configuration's word
  // pattern will be used.
  std::optional<std::string_view> wordPattern;
};

/**
 * @since 3.17.0
 */
struct TypeHierarchyItem {
  // The name of this item.
  std::string_view name;

  // The kind of this item.
  SymbolKind kind;

  // Tags for this item.
  std::optional<std::vector<SymbolTag>> tags;

  // More detail for this item, e.g. the signature of a function.
  std::optional<std::string_view> detail;

  // The resource identifier of this item.
  std::string_view uri;

  // The range enclosing this symbol not including leading/trailing whitespace
  // but everything else, e.g. comments and code.
  Range range;

  // The range that should be selected and revealed when this symbol is being
  // picked, e.g. the name of a function. Must be contained by the
  // {@link TypeHierarchyItem.range `range`}.
  Range selectionRange;

  // A data entry field that is preserved between a type hierarchy prepare and
  // supertypes or subtypes requests. It could also be used to identify the
  // type hierarchy in the server, helping improve the performance on
  // resolving supertypes and subtypes.
  std::optional<LSPAny> data;
};

/**
 * A parameter literal used in inlay hint requests.
 *
 * @since 3.17.0
 */
struct InlayHintParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The document range for which inlay hints should be computed.
  Range range;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
};

/**
 * An inline completion item represents a text snippet that is proposed inline to complete text that is being typed.
 *
 * @since 3.18.0
 * @proposed
 */
struct InlineCompletionItem {
  // The text to replace the range with. Must be set.
  std::variant<std::string_view, StringValue> insertText;

  // A text that is used to decide if this inline completion should be shown. When `falsy` the {@link
  // InlineCompletionItem.insertText} is used.
  std::optional<std::string_view> filterText;

  // The range to replace. Must begin and end on the same line.
  std::optional<Range> range;

  // An optional {@link Command} that is executed *after* inserting this completion.
  std::optional<Command> command;
};

/**
 * A text edit applicable to a text document.
 */
struct TextEdit {
  // The range of the text document to be manipulated. To insert
  // text into a document create a range where start === end.
  Range range;

  // The string to be inserted. For delete operations use an
  // empty string.
  std::string_view newText;
};

/**
 * The result of a hover request.
 */
struct Hover {
  // The hover's content
  std::variant<MarkupContent, MarkedString, std::vector<MarkedString>> contents;

  // An optional range inside the text document that is used to
  // visualize the hover, e.g. by changing the background color.
  std::optional<Range> range;
};

/**
 * A document highlight is a range inside a text document which deserves
 * special attention. Usually a document highlight is visualized by changing
 * the background color of its range.
 */
struct DocumentHighlight {
  // The range this highlight applies to.
  Range range;

  // The highlight kind, default is {@link DocumentHighlightKind.Text text}.
  std::optional<DocumentHighlightKind> kind;
};

/**
 * Represents programming constructs like variables, classes, interfaces etc.
 * that appear in a document. Document symbols can be hierarchical and they
 * have two ranges: one that encloses its definition and one that points to
 * its most interesting range, e.g. the range of an identifier.
 */
struct DocumentSymbol {
  // The name of this symbol. Will be displayed in the user interface and therefore must not be
  // an empty string or a string only consisting of white spaces.
  std::string_view name;

  // More detail for this symbol, e.g the signature of a function.
  std::optional<std::string_view> detail;

  // The kind of this symbol.
  SymbolKind kind;

  // Tags for this document symbol.
  //
  // @since 3.16.0
  std::optional<std::vector<SymbolTag>> tags;

  // Indicates if this symbol is deprecated.
  //
  // @deprecated Use tags instead
  std::optional<bool> deprecated;

  // The range enclosing this symbol not including leading/trailing whitespace but everything else
  // like comments. This information is typically used to determine if the clients cursor is
  // inside the symbol to reveal in the symbol in the UI.
  Range range;

  // The range that should be selected and revealed when this symbol is being picked, e.g the name of a function.
  // Must be contained by the `range`.
  Range selectionRange;

  // Children of this symbol, e.g. properties of a class.
  std::optional<std::vector<DocumentSymbol>> children;
};

/**
 * A code lens represents a {@link Command command} that should be shown along with
 * source text, like the number of references, a way to run tests, etc.
 *
 * A code lens is _unresolved_ when no command is associated to it. For performance
 * reasons the creation of a code lens and resolving should be done in two stages.
 */
struct CodeLens {
  // The range in which this code lens is valid. Should only span a single line.
  Range range;

  // The command this code lens represents.
  std::optional<Command> command;

  // A data entry field that is preserved on a code lens item between
  // a {@link CodeLensRequest} and a {@link CodeLensResolveRequest}
  std::optional<LSPAny> data;
};

/**
 * A document link is a range in a text document that links to an internal or external resource, like another
 * text document or a web site.
 */
struct DocumentLink {
  // The range this link applies to.
  Range range;

  // The uri this link points to. If missing a resolve request is sent later.
  std::optional<std::string_view> target;

  // The tooltip text when you hover over this link.
  //
  // If a tooltip is provided, is will be displayed in a string that includes instructions on how to
  // trigger the link, such as `{0} (ctrl + click)`. The specific instructions vary depending on OS,
  // user settings, and localization.
  //
  // @since 3.15.0
  std::optional<std::string_view> tooltip;

  // A data entry field that is preserved on a document link between a
  // DocumentLinkRequest and a DocumentLinkResolveRequest.
  std::optional<LSPAny> data;
};

/**
 * The parameters of a {@link DocumentRangeFormattingRequest}.
 */
struct DocumentRangeFormattingParams {
  // The document to format.
  TextDocumentIdentifier textDocument;

  // The range to format
  Range range;

  // The format options
  FormattingOptions options;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
};

/**
 * The parameters of a {@link DocumentRangesFormattingRequest}.
 *
 * @since 3.18.0
 * @proposed
 */
struct DocumentRangesFormattingParams {
  // The document to format.
  TextDocumentIdentifier textDocument;

  // The ranges to format
  std::vector<Range> ranges;

  // The format options
  FormattingOptions options;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
};

/**
 * Represents the connection of two locations. Provides additional metadata over normal {@link Location locations},
 * including an origin range.
 */
struct LocationLink {
  // Span of the origin of this link.
  //
  // Used as the underlined span for mouse interaction. Defaults to the word range at
  // the definition position.
  std::optional<Range> originSelectionRange;

  // The target resource identifier of this link.
  std::string_view targetUri;

  // The full target range of this link. If the target for example is a symbol then target range is the
  // range enclosing this symbol not including leading/trailing whitespace but everything else
  // like comments. This information is typically used to highlight the range in the editor.
  Range targetRange;

  // The range that should be selected and revealed when this link is being followed, e.g the name of a function.
  // Must be contained by the `targetRange`. See also `DocumentSymbol#range`
  Range targetSelectionRange;
};

/**
 * @since 3.17.0
 */
struct InlineValueContext {
  // The stack frame (as a DAP Id) where the execution has stopped.
  std::int32_t frameId;

  // The document range where execution has stopped.
  // Typically the end position of the range denotes the line where the inline values are shown.
  Range stoppedLocation;
};

/**
 * Provide inline value as text.
 *
 * @since 3.17.0
 */
struct InlineValueText {
  // The document range for which the inline value applies.
  Range range;

  // The text of the inline value.
  std::string text;
};

/**
 * Provide inline value through a variable lookup.
 * If only a range is specified, the variable name will be extracted from the underlying document.
 * An optional variable name can be used to override the extracted name.
 *
 * @since 3.17.0
 */
struct InlineValueVariableLookup {
  // The document range for which the inline value applies.
  // The range is used to extract the variable name from the underlying document.
  Range range;

  // If specified the name of the variable to look up.
  std::optional<std::string_view> variableName;

  // How to perform the lookup.
  bool caseSensitiveLookup;
};

/**
 * Provide an inline value through an expression evaluation.
 * If only a range is specified, the expression will be extracted from the underlying document.
 * An optional expression can be used to override the extracted expression.
 *
 * @since 3.17.0
 */
struct InlineValueEvaluatableExpression {
  // The document range for which the inline value applies.
  // The range is used to extract the evaluatable expression from the underlying document.
  Range range;

  // If specified the expression overrides the extracted expression.
  std::optional<std::string_view> expression;
};

/**
 * A special text edit to provide an insert and a replace operation.
 *
 * @since 3.16.0
 */
struct InsertReplaceEdit {
  // The string to be inserted.
  std::string_view newText;

  // The range if the insert is requested
  Range insert;

  // The range if the replace is requested.
  Range replace;
};

/**
 * @since 3.18.0
 */
struct PrepareRenamePlaceholder {
  Range range;
  std::string_view placeholder;
};

/**
 * An interactive text edit.
 *
 * @since 3.18.0
 * @proposed
 */
struct SnippetTextEdit {
  // The range of the text document to be manipulated.
  Range range;

  // The snippet to be inserted.
  StringValue snippet;

  // The actual identifier of the snippet edit.
  std::optional<ChangeAnnotationIdentifier> annotationId;
};

/**
 * Describes the currently selected completion item.
 *
 * @since 3.18.0
 * @proposed
 */
struct SelectedCompletionInfo {
  // The range that will be replaced if this completion item is accepted.
  Range range;

  // The text the range will be replaced with if this completion is accepted.
  std::string text;
};

/**
 * @since 3.18.0
 */
struct TextDocumentContentChangePartial {
  // The range of the document that changed.
  Range range;

  // The optional length of the range that got replaced.
  //
  // @deprecated use range instead.
  std::optional<std::uint32_t> rangeLength;

  // The new text for the provided range.
  std::string text;
};

/**
 * Edit range variant that includes ranges for insert and replace operations.
 *
 * @since 3.18.0
 */
struct EditRangeWithInsertReplace {
  Range insert;
  Range replace;
};

/**
 * A filter to describe in which file operation requests or notifications
 * the server is interested in receiving.
 *
 * @since 3.16.0
 */
struct FileOperationFilter {
  // A Uri scheme like `file` or `untitled`.
  std::optional<std::string_view> scheme;

  // The actual file operation pattern.
  FileOperationPattern pattern;
};

struct WindowClientCapabilities {
  // It indicates whether the client supports server initiated
  // progress using the `window/workDoneProgress/create` request.
  //
  // The capability also controls Whether client supports handling
  // of progress notifications. If set servers are allowed to report a
  // `workDoneProgress` property in the request specific server
  // capabilities.
  //
  // @since 3.15.0
  std::optional<bool> workDoneProgress;

  // Capabilities specific to the showMessage request.
  //
  // @since 3.16.0
  std::optional<ShowMessageRequestClientCapabilities> showMessage;

  // Capabilities specific to the showDocument request.
  //
  // @since 3.16.0
  std::optional<ShowDocumentClientCapabilities> showDocument;
};

/**
 * Client Capabilities for a {@link SignatureHelpRequest}.
 */
struct SignatureHelpClientCapabilities {
  // Whether signature help supports dynamic registration.
  std::optional<bool> dynamicRegistration;

  // The client supports the following `SignatureInformation`
  // specific properties.
  std::optional<ClientSignatureInformationOptions> signatureInformation;

  // The client supports to send additional context information for a
  // `textDocument/signatureHelp` request. A client that opts into
  // contextSupport will also support the `retriggerCharacters` on
  // `SignatureHelpOptions`.
  //
  // @since 3.15.0
  std::optional<bool> contextSupport;
};

/**
 * @since 3.16.0
 */
struct SemanticTokensClientCapabilities {
  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)`
  // return value for the corresponding server capability as well.
  std::optional<bool> dynamicRegistration;

  // Which requests the client supports and might send to the server
  // depending on the server's capability. Please note that clients might not
  // show semantic tokens or degrade some of the user experience if a range
  // or full request is advertised by the client but not provided by the
  // server. If for example the client capability `requests.full` and
  // `request.range` are both set to true but the server only provides a
  // range provider the client might not render a minimap correctly or might
  // even decide to not show any semantic tokens at all.
  ClientSemanticTokensRequestOptions requests;

  // The token types that the client supports.
  std::vector<std::string_view> tokenTypes;

  // The token modifiers that the client supports.
  std::vector<std::string_view> tokenModifiers;

  // The token formats the clients supports.
  std::vector<TokenFormat> formats;

  // Whether the client supports tokens that can overlap each other.
  std::optional<bool> overlappingTokenSupport;

  // Whether the client supports tokens that can span multiple lines.
  std::optional<bool> multilineTokenSupport;

  // Whether the client allows the server to actively cancel a
  // semantic token request, e.g. supports returning
  // LSPErrorCodes.ServerCancelled. If a server does the client
  // needs to retrigger the request.
  //
  // @since 3.17.0
  std::optional<bool> serverCancelSupport;

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
  std::optional<bool> augmentsSyntaxTokens;
};

/**
 * Workspace specific client capabilities.
 */
struct WorkspaceClientCapabilities {
  // The client supports applying batch edits
  // to the workspace by supporting the request
  // 'workspace/applyEdit'
  std::optional<bool> applyEdit;

  // Capabilities specific to `WorkspaceEdit`s.
  std::optional<WorkspaceEditClientCapabilities> workspaceEdit;

  // Capabilities specific to the `workspace/didChangeConfiguration` notification.
  std::optional<DidChangeConfigurationClientCapabilities> didChangeConfiguration;

  // Capabilities specific to the `workspace/didChangeWatchedFiles` notification.
  std::optional<DidChangeWatchedFilesClientCapabilities> didChangeWatchedFiles;

  // Capabilities specific to the `workspace/symbol` request.
  std::optional<WorkspaceSymbolClientCapabilities> symbol;

  // Capabilities specific to the `workspace/executeCommand` request.
  std::optional<ExecuteCommandClientCapabilities> executeCommand;

  // The client has support for workspace folders.
  //
  // @since 3.6.0
  std::optional<bool> workspaceFolders;

  // The client supports `workspace/configuration` requests.
  //
  // @since 3.6.0
  std::optional<bool> configuration;

  // Capabilities specific to the semantic token requests scoped to the
  // workspace.
  //
  // @since 3.16.0.
  std::optional<SemanticTokensWorkspaceClientCapabilities> semanticTokens;

  // Capabilities specific to the code lens requests scoped to the
  // workspace.
  //
  // @since 3.16.0.
  std::optional<CodeLensWorkspaceClientCapabilities> codeLens;

  // The client has support for file notifications/requests for user operations on files.
  //
  // Since 3.16.0
  std::optional<FileOperationClientCapabilities> fileOperations;

  // Capabilities specific to the inline values requests scoped to the
  // workspace.
  //
  // @since 3.17.0.
  std::optional<InlineValueWorkspaceClientCapabilities> inlineValue;

  // Capabilities specific to the inlay hint requests scoped to the
  // workspace.
  //
  // @since 3.17.0.
  std::optional<InlayHintWorkspaceClientCapabilities> inlayHint;

  // Capabilities specific to the diagnostic requests scoped to the
  // workspace.
  //
  // @since 3.17.0.
  std::optional<DiagnosticWorkspaceClientCapabilities> diagnostics;

  // Capabilities specific to the folding range requests scoped to the workspace.
  //
  // @since 3.18.0
  // @proposed
  std::optional<FoldingRangeWorkspaceClientCapabilities> foldingRange;

  // Capabilities specific to the `workspace/textDocumentContent` request.
  //
  // @since 3.18.0
  // @proposed
  std::optional<TextDocumentContentClientCapabilities> textDocumentContent;
};

/**
 * Completion client capabilities
 */
struct CompletionClientCapabilities {
  // Whether completion supports dynamic registration.
  std::optional<bool> dynamicRegistration;

  // The client supports the following `CompletionItem` specific
  // capabilities.
  std::optional<ClientCompletionItemOptions> completionItem;
  std::optional<ClientCompletionItemOptionsKind> completionItemKind;

  // Defines how the client handles whitespace and indentation
  // when accepting a completion item that uses multi line
  // text in either `insertText` or `textEdit`.
  //
  // @since 3.17.0
  std::optional<InsertTextMode> insertTextMode;

  // The client supports to send additional context information for a
  // `textDocument/completion` request.
  std::optional<bool> contextSupport;

  // The client supports the following `CompletionList` specific
  // capabilities.
  //
  // @since 3.17.0
  std::optional<CompletionListCapabilities> completionList;
};

/**
 * The Client Capabilities of a {@link CodeActionRequest}.
 */
struct CodeActionClientCapabilities {
  // Whether code action supports dynamic registration.
  std::optional<bool> dynamicRegistration;

  // The client support code action literals of type `CodeAction` as a valid
  // response of the `textDocument/codeAction` request. If the property is not
  // set the request can only return `Command` literals.
  //
  // @since 3.8.0
  std::optional<ClientCodeActionLiteralOptions> codeActionLiteralSupport;

  // Whether code action supports the `isPreferred` property.
  //
  // @since 3.15.0
  std::optional<bool> isPreferredSupport;

  // Whether code action supports the `disabled` property.
  //
  // @since 3.16.0
  std::optional<bool> disabledSupport;

  // Whether code action supports the `data` property which is
  // preserved between a `textDocument/codeAction` and a
  // `codeAction/resolve` request.
  //
  // @since 3.16.0
  std::optional<bool> dataSupport;

  // Whether the client supports resolving additional code action
  // properties via a separate `codeAction/resolve` request.
  //
  // @since 3.16.0
  std::optional<ClientCodeActionResolveOptions> resolveSupport;

  // Whether the client honors the change annotations in
  // text edits and resource operations returned via the
  // `CodeAction#edit` property by for example presenting
  // the workspace edit in the user interface and asking
  // for confirmation.
  //
  // @since 3.16.0
  std::optional<bool> honorsChangeAnnotations;

  // Whether the client supports documentation for a class of
  // code actions.
  //
  // @since 3.18.0
  // @proposed
  std::optional<bool> documentationSupport;

  // Client supports the tag property on a code action. Clients
  // supporting tags have to handle unknown tags gracefully.
  //
  // @since 3.18.0 - proposed
  std::optional<CodeActionTagOptions> tagSupport;
};

/**
 * Represents the signature of something callable. A signature
 * can have a label, like a function-name, a doc-comment, and
 * a set of parameters.
 */
struct SignatureInformation {
  // The label of this signature. Will be shown in
  // the UI.
  std::string_view label;

  // The human-readable doc-comment of this signature. Will be shown
  // in the UI but can be omitted.
  std::optional<std::variant<std::string_view, MarkupContent>> documentation;

  // The parameters of this signature.
  std::optional<std::vector<ParameterInformation>> parameters;

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
  std::optional<std::uint32_t> activeParameter;
};

/**
 * The publish diagnostic client capabilities.
 */
struct PublishDiagnosticsClientCapabilities {
  // Whether the client interprets the version property of the
  // `textDocument/publishDiagnostics` notification's parameter.
  //
  // @since 3.15.0
  std::optional<bool> versionSupport;

  // Whether the clients accepts diagnostics with related information.
  std::optional<bool> relatedInformation;

  // Client supports the tag property to provide meta data about a diagnostic.
  // Clients supporting tags have to handle unknown tags gracefully.
  //
  // @since 3.15.0
  std::optional<ClientDiagnosticsTagOptions> tagSupport;

  // Client supports a codeDescription property
  //
  // @since 3.16.0
  std::optional<bool> codeDescriptionSupport;

  // Whether code action supports the `data` property which is
  // preserved between a `textDocument/publishDiagnostics` and
  // `textDocument/codeAction` request.
  //
  // @since 3.16.0
  std::optional<bool> dataSupport;
};

/**
 * Client capabilities specific to diagnostic pull requests.
 *
 * @since 3.17.0
 */
struct DiagnosticClientCapabilities {
  // Whether implementation supports dynamic registration. If this is set to `true`
  // the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)`
  // return value for the corresponding server capability as well.
  std::optional<bool> dynamicRegistration;

  // Whether the clients supports related documents for document diagnostic pulls.
  std::optional<bool> relatedDocumentSupport;

  // Whether the clients accepts diagnostics with related information.
  std::optional<bool> relatedInformation;

  // Client supports the tag property to provide meta data about a diagnostic.
  // Clients supporting tags have to handle unknown tags gracefully.
  //
  // @since 3.15.0
  std::optional<ClientDiagnosticsTagOptions> tagSupport;

  // Client supports a codeDescription property
  //
  // @since 3.16.0
  std::optional<bool> codeDescriptionSupport;

  // Whether code action supports the `data` property which is
  // preserved between a `textDocument/publishDiagnostics` and
  // `textDocument/codeAction` request.
  //
  // @since 3.16.0
  std::optional<bool> dataSupport;
};

/**
 * A notebook document.
 *
 * @since 3.17.0
 */
struct NotebookDocument {
  // The notebook document's uri.
  std::string_view uri;

  // The type of the notebook.
  std::string_view notebookType;

  // The version number of this document (it will increase after each
  // change, including undo/redo).
  std::int32_t version;

  // Additional metadata stored with the notebook
  // document.
  //
  // Note: should always be an object literal (e.g. LSPObject)
  std::optional<LSPObject> metadata;

  // The cells of a notebook.
  std::vector<NotebookCell> cells;
};

/**
 * A change describing how to move a `NotebookCell`
 * array from state S to S'.
 *
 * @since 3.17.0
 */
struct NotebookCellArrayChange {
  // The start oftest of the cell that changed.
  std::uint32_t start;

  // The deleted cells
  std::uint32_t deleteCount;

  // The new cells, if any
  std::optional<std::vector<NotebookCell>> cells;
};

/**
 * Provider options for a {@link CodeActionRequest}.
 */
struct CodeActionOptions {
  // CodeActionKinds that this server may return.
  //
  // The list of kinds may be generic, such as `CodeActionKind.Refactor`, or the server
  // may list out every specific kind they provide.
  std::optional<std::vector<CodeActionKind>> codeActionKinds;

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
  std::optional<std::vector<CodeActionKindDocumentation>> documentation;

  // The server provides support to resolve additional
  // information for a code action.
  //
  // @since 3.16.0
  std::optional<bool> resolveProvider;
  std::optional<bool> workDoneProgress;
};

struct FileSystemWatcher {
  // The glob pattern to watch. See {@link GlobPattern glob pattern} for more detail.
  //
  // @since 3.17.0 support for relative patterns.
  GlobPattern globPattern;

  // The kind of events of interest. If omitted it defaults
  // to WatchKind.Create | WatchKind.Change | WatchKind.Delete
  // which is 7.
  std::optional<WatchKind> kind;
};

/**
 * A document filter where `language` is required field.
 *
 * @since 3.18.0
 */
struct TextDocumentFilterLanguage {
  // A language id, like `typescript`.
  std::string_view language;

  // A Uri {@link Uri.scheme scheme}, like `file` or `untitled`.
  std::optional<std::string_view> scheme;

  // A glob pattern, like **​/\*.{ts,js}. See TextDocumentFilter for examples.
  //
  // @since 3.18.0 - support for relative patterns. Whether clients support
  // relative patterns depends on the client capability
  // `textDocuments.filters.relativePatternSupport`.
  std::optional<GlobPattern> pattern;
};

/**
 * A document filter where `scheme` is required field.
 *
 * @since 3.18.0
 */
struct TextDocumentFilterScheme {
  // A language id, like `typescript`.
  std::optional<std::string_view> language;

  // A Uri {@link Uri.scheme scheme}, like `file` or `untitled`.
  std::string_view scheme;

  // A glob pattern, like **​/\*.{ts,js}. See TextDocumentFilter for examples.
  //
  // @since 3.18.0 - support for relative patterns. Whether clients support
  // relative patterns depends on the client capability
  // `textDocuments.filters.relativePatternSupport`.
  std::optional<GlobPattern> pattern;
};

/**
 * A document filter where `pattern` is required field.
 *
 * @since 3.18.0
 */
struct TextDocumentFilterPattern {
  // A language id, like `typescript`.
  std::optional<std::string_view> language;

  // A Uri {@link Uri.scheme scheme}, like `file` or `untitled`.
  std::optional<std::string_view> scheme;

  // A glob pattern, like **​/\*.{ts,js}. See TextDocumentFilter for examples.
  //
  // @since 3.18.0 - support for relative patterns. Whether clients support
  // relative patterns depends on the client capability
  // `textDocuments.filters.relativePatternSupport`.
  GlobPattern pattern;
};

/**
 * A notebook document filter where `notebookType` is required field.
 *
 * @since 3.18.0
 */
struct NotebookDocumentFilterNotebookType {
  // The type of the enclosing notebook.
  std::string_view notebookType;

  // A Uri {@link Uri.scheme scheme}, like `file` or `untitled`.
  std::optional<std::string_view> scheme;

  // A glob pattern.
  std::optional<GlobPattern> pattern;
};

/**
 * A notebook document filter where `scheme` is required field.
 *
 * @since 3.18.0
 */
struct NotebookDocumentFilterScheme {
  // The type of the enclosing notebook.
  std::optional<std::string_view> notebookType;

  // A Uri {@link Uri.scheme scheme}, like `file` or `untitled`.
  std::string_view scheme;

  // A glob pattern.
  std::optional<GlobPattern> pattern;
};

/**
 * A notebook document filter where `pattern` is required field.
 *
 * @since 3.18.0
 */
struct NotebookDocumentFilterPattern {
  // The type of the enclosing notebook.
  std::optional<std::string_view> notebookType;

  // A Uri {@link Uri.scheme scheme}, like `file` or `untitled`.
  std::optional<std::string_view> scheme;

  // A glob pattern.
  GlobPattern pattern;
};

/**
 * The definition of a symbol represented as one or many {@link Location locations}.
 * For most programming languages there is only one location at which a symbol is
 * defined.
 *
 * Servers should prefer returning `DefinitionLink` over `Definition` if supported
 * by the client.
 */
using Definition = std::variant<Location, std::vector<Location>>;

/**
 * The declaration of a symbol representation as one or many {@link Location locations}.
 */
using Declaration = std::variant<Location, std::vector<Location>>;

/**
 * Represents information about programming constructs like variables, classes,
 * interfaces etc.
 */
struct SymbolInformation {
  // Indicates if this symbol is deprecated.
  //
  // @deprecated Use tags instead
  std::optional<bool> deprecated;

  // The location of this symbol. The location's range is used by a tool
  // to reveal the location in the editor. If the symbol is selected in the
  // tool the range's start information is used to position the cursor. So
  // the range usually spans more than the actual symbol's name and does
  // normally include things like visibility modifiers.
  //
  // The range doesn't have to denote a node range in the sense of an abstract
  // syntax tree. It can therefore not be used to re-construct a hierarchy of
  // the symbols.
  Location location;

  // The name of this symbol.
  std::string_view name;

  // The kind of this symbol.
  SymbolKind kind;

  // Tags for this symbol.
  //
  // @since 3.16.0
  std::optional<std::vector<SymbolTag>> tags;

  // The name of the symbol containing this symbol. This information is for
  // user interface purposes (e.g. to render a qualifier in the user interface
  // if necessary). It can't be used to re-infer a hierarchy for the document
  // symbols.
  std::optional<std::string_view> containerName;
};

/**
 * A special workspace symbol that supports locations without a range.
 *
 * See also SymbolInformation.
 *
 * @since 3.17.0
 */
struct WorkspaceSymbol {
  // The location of the symbol. Whether a server is allowed to
  // return a location without a range depends on the client
  // capability `workspace.symbol.resolveSupport`.
  //
  // See SymbolInformation#location for more details.
  std::variant<Location, LocationUriOnly> location;

  // A data entry field that is preserved on a workspace symbol between a
  // workspace symbol request and a workspace symbol resolve request.
  std::optional<LSPAny> data;

  // The name of this symbol.
  std::string_view name;

  // The kind of this symbol.
  SymbolKind kind;

  // Tags for this symbol.
  //
  // @since 3.16.0
  std::optional<std::vector<SymbolTag>> tags;

  // The name of the symbol containing this symbol. This information is for
  // user interface purposes (e.g. to render a qualifier in the user interface
  // if necessary). It can't be used to re-infer a hierarchy for the document
  // symbols.
  std::optional<std::string_view> containerName;
};

/**
 * An inlay hint label part allows for interactive and composite labels
 * of inlay hints.
 *
 * @since 3.17.0
 */
struct InlayHintLabelPart {
  // The value of this label part.
  std::string_view value;

  // The tooltip text when you hover over this label part. Depending on
  // the client capability `inlayHint.resolveSupport` clients might resolve
  // this property late using the resolve request.
  std::optional<std::variant<std::string_view, MarkupContent>> tooltip;

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
  std::optional<Location> location;

  // An optional command for this label part.
  //
  // Depending on the client capability `inlayHint.resolveSupport` clients
  // might resolve this property late using the resolve request.
  std::optional<Command> command;
};

/**
 * Represents a related message and source code location for a diagnostic. This should be
 * used to point to code locations that cause or related to a diagnostics, e.g when duplicating
 * a symbol in a scope.
 */
struct DiagnosticRelatedInformation {
  // The location of this related diagnostic information.
  Location location;

  // The message of this related diagnostic information.
  std::string_view message;
};

/**
 * The parameter of a `callHierarchy/incomingCalls` request.
 *
 * @since 3.16.0
 */
struct CallHierarchyIncomingCallsParams {
  CallHierarchyItem item;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * Represents an incoming call, e.g. a caller of a method or constructor.
 *
 * @since 3.16.0
 */
struct CallHierarchyIncomingCall {
  // The item that makes the call.
  CallHierarchyItem from;

  // The ranges at which the calls appear. This is relative to the caller
  // denoted by {@link CallHierarchyIncomingCall.from `this.from`}.
  std::vector<Range> fromRanges;
};

/**
 * The parameter of a `callHierarchy/outgoingCalls` request.
 *
 * @since 3.16.0
 */
struct CallHierarchyOutgoingCallsParams {
  CallHierarchyItem item;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * Represents an outgoing call, e.g. calling a getter from a method or a method from a constructor etc.
 *
 * @since 3.16.0
 */
struct CallHierarchyOutgoingCall {
  // The item that is called.
  CallHierarchyItem to;

  // The range at which this item is called. This is the range relative to the caller, e.g the item
  // passed to {@link CallHierarchyItemProvider.provideCallHierarchyOutgoingCalls `provideCallHierarchyOutgoingCalls`}
  // and not {@link CallHierarchyOutgoingCall.to `this.to`}.
  std::vector<Range> fromRanges;
};

/**
 * The parameter of a `typeHierarchy/supertypes` request.
 *
 * @since 3.17.0
 */
struct TypeHierarchySupertypesParams {
  TypeHierarchyItem item;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * The parameter of a `typeHierarchy/subtypes` request.
 *
 * @since 3.17.0
 */
struct TypeHierarchySubtypesParams {
  TypeHierarchyItem item;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * Represents a collection of {@link InlineCompletionItem inline completion items} to be presented in the editor.
 *
 * @since 3.18.0
 * @proposed
 */
struct InlineCompletionList {
  // The inline completion items
  std::vector<InlineCompletionItem> items;
};

struct ColorPresentation {
  // The label of this color presentation. It will be shown on the color
  // picker header. By default this is also the text that is inserted when selecting
  // this color presentation.
  std::string_view label;

  // An {@link TextEdit edit} which is applied to a document when selecting
  // this presentation for the color.  When `falsy` the {@link ColorPresentation.label label}
  // is used.
  std::optional<TextEdit> textEdit;

  // An optional array of additional {@link TextEdit text edits} that are applied when
  // selecting this color presentation. Edits must not overlap with the main {@link ColorPresentation.textEdit edit} nor
  // with themselves.
  std::optional<std::vector<TextEdit>> additionalTextEdits;
};

/**
 * A special text edit with an additional change annotation.
 *
 * @since 3.16.0.
 */
struct AnnotatedTextEdit {
  // The actual identifier of the change annotation
  ChangeAnnotationIdentifier annotationId;

  // The range of the text document to be manipulated. To insert
  // text into a document create a range where start === end.
  Range range;

  // The string to be inserted. For delete operations use an
  // empty string.
  std::string_view newText;
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
struct InlineValueParams {
  // The text document.
  TextDocumentIdentifier textDocument;

  // The document range for which inline values should be computed.
  Range range;

  // Additional information about the context in which inline values were
  // requested.
  InlineValueContext context;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
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
using InlineValue = std::variant<InlineValueText, InlineValueVariableLookup, InlineValueEvaluatableExpression>;

/**
 * A completion item represents a text snippet that is
 * proposed to complete text that is being typed.
 */
struct CompletionItem {
  // The label of this completion item.
  //
  // The label property is also by default the text that
  // is inserted when selecting this completion.
  //
  // If label details are provided the label itself should
  // be an unqualified name of the completion item.
  std::string_view label;

  // Additional details for the label
  //
  // @since 3.17.0
  std::optional<CompletionItemLabelDetails> labelDetails;

  // The kind of this completion item. Based of the kind
  // an icon is chosen by the editor.
  std::optional<CompletionItemKind> kind;

  // Tags for this completion item.
  //
  // @since 3.15.0
  std::optional<std::vector<CompletionItemTag>> tags;

  // A human-readable string with additional information
  // about this item, like type or symbol information.
  std::optional<std::string_view> detail;

  // A human-readable string that represents a doc-comment.
  std::optional<std::variant<std::string_view, MarkupContent>> documentation;

  // Indicates if this item is deprecated.
  // @deprecated Use `tags` instead.
  std::optional<bool> deprecated;

  // Select this item when showing.
  //
  // *Note* that only one completion item can be selected and that the
  // tool / client decides which item that is. The rule is that the *first*
  // item of those that match best is selected.
  std::optional<bool> preselect;

  // A string that should be used when comparing this item
  // with other items. When `falsy` the {@link CompletionItem.label label}
  // is used.
  std::optional<std::string_view> sortText;

  // A string that should be used when filtering a set of
  // completion items. When `falsy` the {@link CompletionItem.label label}
  // is used.
  std::optional<std::string_view> filterText;

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
  std::optional<std::string_view> insertText;

  // The format of the insert text. The format applies to both the
  // `insertText` property and the `newText` property of a provided
  // `textEdit`. If omitted defaults to `InsertTextFormat.PlainText`.
  //
  // Please note that the insertTextFormat doesn't apply to
  // `additionalTextEdits`.
  std::optional<InsertTextFormat> insertTextFormat;

  // How whitespace and indentation is handled during completion
  // item insertion. If not provided the clients default value depends on
  // the `textDocument.completion.insertTextMode` client capability.
  //
  // @since 3.16.0
  std::optional<InsertTextMode> insertTextMode;

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
  std::optional<std::variant<TextEdit, InsertReplaceEdit>> textEdit;

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
  std::optional<std::string_view> textEditText;

  // An optional array of additional {@link TextEdit text edits} that are applied when
  // selecting this completion. Edits must not overlap (including the same insert position)
  // with the main {@link CompletionItem.textEdit edit} nor with themselves.
  //
  // Additional text edits should be used to change text unrelated to the current cursor position
  // (for example adding an import statement at the top of the file if the completion item will
  // insert an unqualified type).
  std::optional<std::vector<TextEdit>> additionalTextEdits;

  // An optional set of characters that when pressed while this completion is active will accept it first and
  // then type that character. *Note* that all commit characters should have `length=1` and that superfluous
  // characters will be ignored.
  std::optional<std::vector<std::string_view>> commitCharacters;

  // An optional {@link Command command} that is executed *after* inserting this completion. *Note* that
  // additional modifications to the current document should be described with the
  // {@link CompletionItem.additionalTextEdits additionalTextEdits}-property.
  std::optional<Command> command;

  // A data entry field that is preserved on a completion item between a
  // {@link CompletionRequest} and a {@link CompletionResolveRequest}.
  std::optional<LSPAny> data;
};

using PrepareRenameResult = std::variant<Range, PrepareRenamePlaceholder, PrepareRenameDefaultBehavior>;

/**
 * Provides information about the context in which an inline completion was requested.
 *
 * @since 3.18.0
 * @proposed
 */
struct InlineCompletionContext {
  // Describes how the inline completion was triggered.
  InlineCompletionTriggerKind triggerKind;

  // Provides information about the currently selected item in the autocomplete widget if it is visible.
  std::optional<SelectedCompletionInfo> selectedCompletionInfo;
};

/**
 * An event describing a change to a text document. If only a text is provided
 * it is considered to be the full content of the document.
 */
using TextDocumentContentChangeEvent =
    std::variant<TextDocumentContentChangePartial, TextDocumentContentChangeWholeDocument>;

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
struct CompletionItemDefaults {
  // A default commit character set.
  //
  // @since 3.17.0
  std::optional<std::vector<std::string_view>> commitCharacters;

  // A default edit range.
  //
  // @since 3.17.0
  std::optional<std::variant<Range, EditRangeWithInsertReplace>> editRange;

  // A default insert text format.
  //
  // @since 3.17.0
  std::optional<InsertTextFormat> insertTextFormat;

  // A default insert text mode.
  //
  // @since 3.17.0
  std::optional<InsertTextMode> insertTextMode;

  // A default data value.
  //
  // @since 3.17.0
  std::optional<LSPAny> data;
};

/**
 * The options to register for file operations.
 *
 * @since 3.16.0
 */
struct FileOperationRegistrationOptions {
  // The actual filters.
  std::vector<FileOperationFilter> filters;
};

/**
 * Signature help represents the signature of something
 * callable. There can be multiple signature but only one
 * active and only one active parameter.
 */
struct SignatureHelp {
  // One or more signatures.
  std::vector<SignatureInformation> signatures;

  // The active signature. If omitted or the value lies outside the
  // range of `signatures` the value defaults to zero or is ignored if
  // the `SignatureHelp` has no signatures.
  //
  // Whenever possible implementors should make an active decision about
  // the active signature and shouldn't rely on a default value.
  //
  // In future version of the protocol this property might become
  // mandatory to better express this.
  std::optional<std::uint32_t> activeSignature;

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
  std::optional<std::uint32_t> activeParameter;
};

/**
 * Text document specific client capabilities.
 */
struct TextDocumentClientCapabilities {
  // Defines which synchronization capabilities the client supports.
  std::optional<TextDocumentSyncClientCapabilities> synchronization;

  // Defines which filters the client supports.
  //
  // @since 3.18.0
  std::optional<TextDocumentFilterClientCapabilities> filters;

  // Capabilities specific to the `textDocument/completion` request.
  std::optional<CompletionClientCapabilities> completion;

  // Capabilities specific to the `textDocument/hover` request.
  std::optional<HoverClientCapabilities> hover;

  // Capabilities specific to the `textDocument/signatureHelp` request.
  std::optional<SignatureHelpClientCapabilities> signatureHelp;

  // Capabilities specific to the `textDocument/declaration` request.
  //
  // @since 3.14.0
  std::optional<DeclarationClientCapabilities> declaration;

  // Capabilities specific to the `textDocument/definition` request.
  std::optional<DefinitionClientCapabilities> definition;

  // Capabilities specific to the `textDocument/typeDefinition` request.
  //
  // @since 3.6.0
  std::optional<TypeDefinitionClientCapabilities> typeDefinition;

  // Capabilities specific to the `textDocument/implementation` request.
  //
  // @since 3.6.0
  std::optional<ImplementationClientCapabilities> implementation;

  // Capabilities specific to the `textDocument/references` request.
  std::optional<ReferenceClientCapabilities> references;

  // Capabilities specific to the `textDocument/documentHighlight` request.
  std::optional<DocumentHighlightClientCapabilities> documentHighlight;

  // Capabilities specific to the `textDocument/documentSymbol` request.
  std::optional<DocumentSymbolClientCapabilities> documentSymbol;

  // Capabilities specific to the `textDocument/codeAction` request.
  std::optional<CodeActionClientCapabilities> codeAction;

  // Capabilities specific to the `textDocument/codeLens` request.
  std::optional<CodeLensClientCapabilities> codeLens;

  // Capabilities specific to the `textDocument/documentLink` request.
  std::optional<DocumentLinkClientCapabilities> documentLink;

  // Capabilities specific to the `textDocument/documentColor` and the
  // `textDocument/colorPresentation` request.
  //
  // @since 3.6.0
  std::optional<DocumentColorClientCapabilities> colorProvider;

  // Capabilities specific to the `textDocument/formatting` request.
  std::optional<DocumentFormattingClientCapabilities> formatting;

  // Capabilities specific to the `textDocument/rangeFormatting` request.
  std::optional<DocumentRangeFormattingClientCapabilities> rangeFormatting;

  // Capabilities specific to the `textDocument/onTypeFormatting` request.
  std::optional<DocumentOnTypeFormattingClientCapabilities> onTypeFormatting;

  // Capabilities specific to the `textDocument/rename` request.
  std::optional<RenameClientCapabilities> rename;

  // Capabilities specific to the `textDocument/foldingRange` request.
  //
  // @since 3.10.0
  std::optional<FoldingRangeClientCapabilities> foldingRange;

  // Capabilities specific to the `textDocument/selectionRange` request.
  //
  // @since 3.15.0
  std::optional<SelectionRangeClientCapabilities> selectionRange;

  // Capabilities specific to the `textDocument/publishDiagnostics` notification.
  std::optional<PublishDiagnosticsClientCapabilities> publishDiagnostics;

  // Capabilities specific to the various call hierarchy requests.
  //
  // @since 3.16.0
  std::optional<CallHierarchyClientCapabilities> callHierarchy;

  // Capabilities specific to the various semantic token request.
  //
  // @since 3.16.0
  std::optional<SemanticTokensClientCapabilities> semanticTokens;

  // Capabilities specific to the `textDocument/linkedEditingRange` request.
  //
  // @since 3.16.0
  std::optional<LinkedEditingRangeClientCapabilities> linkedEditingRange;

  // Client capabilities specific to the `textDocument/moniker` request.
  //
  // @since 3.16.0
  std::optional<MonikerClientCapabilities> moniker;

  // Capabilities specific to the various type hierarchy requests.
  //
  // @since 3.17.0
  std::optional<TypeHierarchyClientCapabilities> typeHierarchy;

  // Capabilities specific to the `textDocument/inlineValue` request.
  //
  // @since 3.17.0
  std::optional<InlineValueClientCapabilities> inlineValue;

  // Capabilities specific to the `textDocument/inlayHint` request.
  //
  // @since 3.17.0
  std::optional<InlayHintClientCapabilities> inlayHint;

  // Capabilities specific to the diagnostic pull model.
  //
  // @since 3.17.0
  std::optional<DiagnosticClientCapabilities> diagnostic;

  // Client capabilities specific to inline completions.
  //
  // @since 3.18.0
  // @proposed
  std::optional<InlineCompletionClientCapabilities> inlineCompletion;
};

/**
 * The params sent in an open notebook document notification.
 *
 * @since 3.17.0
 */
struct DidOpenNotebookDocumentParams {
  // The notebook document that got opened.
  NotebookDocument notebookDocument;

  // The text documents that represent the content
  // of a notebook cell.
  std::vector<TextDocumentItem> cellTextDocuments;
};

/**
 * Structural changes to cells in a notebook document.
 *
 * @since 3.18.0
 */
struct NotebookDocumentCellChangeStructure {
  // The change to the cell array.
  NotebookCellArrayChange array;

  // Additional opened cell text documents.
  std::optional<std::vector<TextDocumentItem>> didOpen;

  // Additional closed cell text documents.
  std::optional<std::vector<TextDocumentIdentifier>> didClose;
};

/**
 * Describe options to be used when registered for text document change events.
 */
struct DidChangeWatchedFilesRegistrationOptions {
  // The watchers to register.
  std::vector<FileSystemWatcher> watchers;
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
 * - `{}` to group sub patterns into an OR expression. (e.g. `**​/\*.{ts,js}` matches all TypeScript and JavaScript
 * files)
 * - `[]` to declare a range of characters to match in a path segment (e.g., `example.[0-9]` to match on `example.0`,
 * `example.1`, …)
 * - `[!...]` to negate a range of characters to match in a path segment (e.g., `example.[!0-9]` to match on
 * `example.a`, `example.b`, but not `example.0`)
 *
 * @sample A language filter that applies to typescript files on disk: `{ language: 'typescript', scheme: 'file' }`
 * @sample A language filter that applies to all package.json paths: `{ language: 'json', pattern: '**package.json' }`
 *
 * @since 3.17.0
 */
using TextDocumentFilter =
    std::variant<TextDocumentFilterLanguage, TextDocumentFilterScheme, TextDocumentFilterPattern>;

/**
 * A notebook document filter denotes a notebook document by
 * different properties. The properties will be match
 * against the notebook's URI (same as with documents)
 *
 * @since 3.17.0
 */
using NotebookDocumentFilter =
    std::variant<NotebookDocumentFilterNotebookType, NotebookDocumentFilterScheme, NotebookDocumentFilterPattern>;

/**
 * Inlay hint information.
 *
 * @since 3.17.0
 */
struct InlayHint {
  // The position of this hint.
  //
  // If multiple hints have the same position, they will be shown in the order
  // they appear in the response.
  Position position;

  // The label of this hint. A human readable string or an array of
  // InlayHintLabelPart label parts.
  //
  // *Note* that neither the string nor the label part can be empty.
  std::variant<std::string_view, std::vector<InlayHintLabelPart>> label;

  // The kind of this hint. Can be omitted in which case the client
  // should fall back to a reasonable default.
  std::optional<InlayHintKind> kind;

  // Optional text edits that are performed when accepting this inlay hint.
  //
  // *Note* that edits are expected to change the document so that the inlay
  // hint (or its nearest variant) is now part of the document and the inlay
  // hint itself is now obsolete.
  std::optional<std::vector<TextEdit>> textEdits;

  // The tooltip text when you hover over this item.
  std::optional<std::variant<std::string_view, MarkupContent>> tooltip;

  // Render padding before the hint.
  //
  // Note: Padding should use the editor's background color, not the
  // background color of the hint itself. That means padding can be used
  // to visually align/separate an inlay hint.
  std::optional<bool> paddingLeft;

  // Render padding after the hint.
  //
  // Note: Padding should use the editor's background color, not the
  // background color of the hint itself. That means padding can be used
  // to visually align/separate an inlay hint.
  std::optional<bool> paddingRight;

  // A data entry field that is preserved on an inlay hint between
  // a `textDocument/inlayHint` and a `inlayHint/resolve` request.
  std::optional<LSPAny> data;
};

/**
 * Represents a diagnostic, such as a compiler error or warning. Diagnostic objects
 * are only valid in the scope of a resource.
 */
struct Diagnostic {
  // The range at which the message applies
  Range range;

  // The diagnostic's severity. To avoid interpretation mismatches when a
  // server is used with different clients it is highly recommended that servers
  // always provide a severity value.
  std::optional<DiagnosticSeverity> severity;

  // The diagnostic's code, which usually appear in the user interface.
  std::optional<std::variant<std::int32_t, std::string_view>> code;

  // An optional property to describe the error code.
  // Requires the code field (above) to be present/not null.
  //
  // @since 3.16.0
  std::optional<CodeDescription> codeDescription;

  // A human-readable string describing the source of this
  // diagnostic, e.g. 'typescript' or 'super lint'. It usually
  // appears in the user interface.
  std::optional<std::string_view> source;

  // The diagnostic's message. It usually appears in the user interface
  std::string_view message;

  // Additional metadata about the diagnostic.
  //
  // @since 3.15.0
  std::optional<std::vector<DiagnosticTag>> tags;

  // An array of related diagnostic information, e.g. when symbol-names within
  // a scope collide all definitions can be marked via this property.
  std::optional<std::vector<DiagnosticRelatedInformation>> relatedInformation;

  // A data entry field that is preserved between a `textDocument/publishDiagnostics`
  // notification and `textDocument/codeAction` request.
  //
  // @since 3.16.0
  std::optional<LSPAny> data;
};

/**
 * Describes textual changes on a text document. A TextDocumentEdit describes all changes
 * on a document version Si and after they are applied move the document to version Si+1.
 * So the creator of a TextDocumentEdit doesn't need to sort the array of edits or do any
 * kind of ordering. However the edits must be non overlapping.
 */
struct TextDocumentEdit {
  // The text document to change.
  OptionalVersionedTextDocumentIdentifier textDocument;

  // The edits to be applied.
  //
  // @since 3.16.0 - support for AnnotatedTextEdit. This is guarded using a
  // client capability.
  //
  // @since 3.18.0 - support for SnippetTextEdit. This is guarded using a
  // client capability.
  std::vector<std::variant<TextEdit, AnnotatedTextEdit, SnippetTextEdit>> edits;
};

/**
 * A parameter literal used in inline completion requests.
 *
 * @since 3.18.0
 * @proposed
 */
struct InlineCompletionParams {
  // Additional information about the context in which inline completions were
  // requested.
  InlineCompletionContext context;

  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
};

/**
 * The change text document notification's parameters.
 */
struct DidChangeTextDocumentParams {
  // The document that did change. The version number points
  // to the version after all provided content changes have
  // been applied.
  VersionedTextDocumentIdentifier textDocument;

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
  std::vector<TextDocumentContentChangeEvent> contentChanges;
};

/**
 * Content changes to a cell in a notebook document.
 *
 * @since 3.18.0
 */
struct NotebookDocumentCellContentChanges {
  VersionedTextDocumentIdentifier document;
  std::vector<TextDocumentContentChangeEvent> changes;
};

/**
 * Represents a collection of {@link CompletionItem completion items} to be presented
 * in the editor.
 */
struct CompletionList {
  // This list it not complete. Further typing results in recomputing this list.
  //
  // Recomputed lists have all their items replaced (not appended) in the
  // incomplete completion sessions.
  bool isIncomplete;

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
  std::optional<CompletionItemDefaults> itemDefaults;

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
  std::optional<CompletionItemApplyKinds> applyKind;

  // The completion items.
  std::vector<CompletionItem> items;
};

/**
 * Options for notifications/requests for user operations on files.
 *
 * @since 3.16.0
 */
struct FileOperationOptions {
  // The server is interested in receiving didCreateFiles notifications.
  std::optional<FileOperationRegistrationOptions> didCreate;

  // The server is interested in receiving willCreateFiles requests.
  std::optional<FileOperationRegistrationOptions> willCreate;

  // The server is interested in receiving didRenameFiles notifications.
  std::optional<FileOperationRegistrationOptions> didRename;

  // The server is interested in receiving willRenameFiles requests.
  std::optional<FileOperationRegistrationOptions> willRename;

  // The server is interested in receiving didDeleteFiles file notifications.
  std::optional<FileOperationRegistrationOptions> didDelete;

  // The server is interested in receiving willDeleteFiles file requests.
  std::optional<FileOperationRegistrationOptions> willDelete;
};

/**
 * Additional information about the context in which a signature help request was triggered.
 *
 * @since 3.15.0
 */
struct SignatureHelpContext {
  // Action that caused signature help to be triggered.
  SignatureHelpTriggerKind triggerKind;

  // Character that caused signature help to be triggered.
  //
  // This is undefined when `triggerKind !== SignatureHelpTriggerKind.TriggerCharacter`
  std::optional<std::string_view> triggerCharacter;

  // `true` if signature help was already showing when it was triggered.
  //
  // Retriggers occurs when the signature help is already active and can be caused by actions such as
  // typing a trigger character, a cursor move, or document content changes.
  bool isRetrigger;

  // The currently active `SignatureHelp`.
  //
  // The `activeSignatureHelp` has its `SignatureHelp.activeSignature` field updated based on
  // the user navigating through available signatures.
  std::optional<SignatureHelp> activeSignatureHelp;
};

/**
 * Defines the capabilities provided by the client.
 */
struct ClientCapabilities {
  // Workspace specific client capabilities.
  std::optional<WorkspaceClientCapabilities> workspace;

  // Text document specific client capabilities.
  std::optional<TextDocumentClientCapabilities> textDocument;

  // Capabilities specific to the notebook document support.
  //
  // @since 3.17.0
  std::optional<NotebookDocumentClientCapabilities> notebookDocument;

  // Window specific client capabilities.
  std::optional<WindowClientCapabilities> window;

  // General client capabilities.
  //
  // @since 3.16.0
  std::optional<GeneralClientCapabilities> general;

  // Experimental client capabilities.
  std::optional<LSPAny> experimental;
};

/**
 * @since 3.18.0
 */
struct NotebookDocumentFilterWithNotebook {
  // The notebook to be synced If a string
  // value is provided it matches against the
  // notebook type. '*' matches every notebook.
  std::variant<std::string_view, NotebookDocumentFilter> notebook;

  // The cells of the matching notebook to be synced.
  std::optional<std::vector<NotebookCellLanguage>> cells;
};

/**
 * @since 3.18.0
 */
struct NotebookDocumentFilterWithCells {
  // The notebook to be synced If a string
  // value is provided it matches against the
  // notebook type. '*' matches every notebook.
  std::optional<std::variant<std::string_view, NotebookDocumentFilter>> notebook;

  // The cells of the matching notebook to be synced.
  std::vector<NotebookCellLanguage> cells;
};

/**
 * A notebook cell text document filter denotes a cell text
 * document by different properties.
 *
 * @since 3.17.0
 */
struct NotebookCellTextDocumentFilter {
  // A filter that matches against the notebook
  // containing the notebook cell. If a string
  // value is provided it matches against the
  // notebook type. '*' matches every notebook.
  std::variant<std::string_view, NotebookDocumentFilter> notebook;

  // A language id like `python`.
  //
  // Will be matched against the language id of the
  // notebook cell document. '*' matches every language.
  std::optional<std::string_view> language;
};

/**
 * The publish diagnostic notification's parameters.
 */
struct PublishDiagnosticsParams {
  // The URI for which diagnostic information is reported.
  std::string_view uri;

  // Optional the version number of the document the diagnostics are published for.
  //
  // @since 3.15.0
  std::optional<std::int32_t> version;

  // An array of diagnostic information items.
  std::vector<Diagnostic> diagnostics;
};

/**
 * A diagnostic report with a full set of problems.
 *
 * @since 3.17.0
 */
struct FullDocumentDiagnosticReport {
  // A full document diagnostic report.
  const std::string_view kind;

  // An optional result id. If provided it will
  // be sent on the next diagnostic request for the
  // same document.
  std::optional<std::string_view> resultId;

  // The actual items.
  std::vector<Diagnostic> items;
};

/**
 * Contains additional diagnostic information about the context in which
 * a {@link CodeActionProvider.provideCodeActions code action} is run.
 */
struct CodeActionContext {
  // An array of diagnostics known on the client side overlapping the range provided to the
  // `textDocument/codeAction` request. They are provided so that the server knows which
  // errors are currently presented to the user for the given range. There is no guarantee
  // that these accurately reflect the error state of the resource. The primary parameter
  // to compute code actions is the provided range.
  std::vector<Diagnostic> diagnostics;

  // Requested kind of actions to return.
  //
  // Actions not of this kind are filtered out by the client before being shown. So servers
  // can omit computing them.
  std::optional<std::vector<CodeActionKind>> only;

  // The reason why code actions were requested.
  //
  // @since 3.17.0
  std::optional<CodeActionTriggerKind> triggerKind;
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
struct WorkspaceEdit {
  // Holds changes to existing resources.
  std::optional<std::unordered_map<std::string_view, std::vector<TextEdit>>> changes;

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
  std::optional<std::vector<std::variant<TextDocumentEdit, CreateFile, RenameFile, DeleteFile>>> documentChanges;

  // A map of change annotations that can be referenced in `AnnotatedTextEdit`s or create, rename and
  // delete file / folder operations.
  //
  // Whether clients honor this property depends on the client capability `workspace.changeAnnotationSupport`.
  //
  // @since 3.16.0
  std::optional<std::unordered_map<ChangeAnnotationIdentifier, ChangeAnnotation>> changeAnnotations;
};

/**
 * Cell changes to a notebook document.
 *
 * @since 3.18.0
 */
struct NotebookDocumentCellChanges {
  // Changes to the cell structure to add or
  // remove cells.
  std::optional<NotebookDocumentCellChangeStructure> structure;

  // Changes to notebook cells properties like its
  // kind, execution summary or metadata.
  std::optional<std::vector<NotebookCell>> data;

  // Changes to the text content of notebook cells.
  std::optional<std::vector<NotebookDocumentCellContentChanges>> textContent;
};

/**
 * Defines workspace specific capabilities of the server.
 *
 * @since 3.18.0
 */
struct WorkspaceOptions {
  // The server supports workspace folder.
  //
  // @since 3.6.0
  std::optional<WorkspaceFoldersServerCapabilities> workspaceFolders;

  // The server is interested in notifications/requests for operations on files.
  //
  // @since 3.16.0
  std::optional<FileOperationOptions> fileOperations;

  // The server supports the `workspace/textDocumentContent` request.
  //
  // @since 3.18.0
  // @proposed
  std::optional<std::variant<TextDocumentContentOptions, TextDocumentContentRegistrationOptions>> textDocumentContent;
};

/**
 * Parameters for a {@link SignatureHelpRequest}.
 */
struct SignatureHelpParams {
  // The signature help context. This is only available if the client specifies
  // to send this using the client capability `textDocument.signatureHelp.contextSupport === true`
  //
  // @since 3.15.0
  std::optional<SignatureHelpContext> context;

  // The text document.
  TextDocumentIdentifier textDocument;

  // The position inside the text document.
  Position position;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
};

/**
 * The initialize parameters
 */
struct _InitializeParams {
  // The process Id of the parent process that started
  // the server.
  //
  // Is `null` if the process has not been started by another process.
  // If the parent process is not alive then the server should exit.
  std::int32_t processId;

  // Information about the client
  //
  // @since 3.15.0
  std::optional<ClientInfo> clientInfo;

  // The locale the client is currently showing the user interface
  // in. This must not necessarily be the locale of the operating
  // system.
  //
  // Uses IETF language tags as the value's syntax
  // (See https://en.wikipedia.org/wiki/IETF_language_tag)
  //
  // @since 3.16.0
  std::optional<std::string_view> locale;

  // The rootPath of the workspace. Is null
  // if no folder is open.
  //
  // @deprecated in favour of rootUri.
  std::optional<std::string_view> rootPath;

  // The rootUri of the workspace. Is null if no
  // folder is open. If both `rootPath` and `rootUri` are set
  // `rootUri` wins.
  //
  // @deprecated in favour of workspaceFolders.
  std::string_view rootUri;

  // The capabilities provided by the client (editor or tool)
  ClientCapabilities capabilities;

  // User provided initialization options.
  std::optional<LSPAny> initializationOptions;

  // The initial trace setting. If omitted trace is disabled ('off').
  std::optional<TraceValue> trace;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;
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
struct NotebookDocumentSyncOptions {
  // The notebooks to be synced
  std::vector<std::variant<NotebookDocumentFilterWithNotebook, NotebookDocumentFilterWithCells>> notebookSelector;

  // Whether save notification should be forwarded to
  // the server. Will only be honored if mode === `notebook`.
  std::optional<bool> save;
};

/**
 * A document filter describes a top level text document or
 * a notebook cell document.
 *
 * @since 3.17.0 - support for NotebookCellTextDocumentFilter.
 */
using DocumentFilter = std::variant<TextDocumentFilter, NotebookCellTextDocumentFilter>;

/**
 * A partial result for a document diagnostic report.
 *
 * @since 3.17.0
 */
struct DocumentDiagnosticReportPartialResult {
  std::unordered_map<std::string_view, std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>>
      relatedDocuments;
};

/**
 * A full diagnostic report with a set of related documents.
 *
 * @since 3.17.0
 */
struct RelatedFullDocumentDiagnosticReport {
  // Diagnostics of related documents. This information is useful
  // in programming languages where code in a file A can generate
  // diagnostics in a file B which A depends on. An example of
  // such a language is C/C++ where marco definitions in a file
  // a.cpp and result in errors in a header file b.hpp.
  //
  // @since 3.17.0
  std::optional<std::unordered_map<std::string_view,
                                   std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>>>
      relatedDocuments;

  // A full document diagnostic report.
  const std::string_view kind;

  // An optional result id. If provided it will
  // be sent on the next diagnostic request for the
  // same document.
  std::optional<std::string_view> resultId;

  // The actual items.
  std::vector<Diagnostic> items;
};

/**
 * An unchanged diagnostic report with a set of related documents.
 *
 * @since 3.17.0
 */
struct RelatedUnchangedDocumentDiagnosticReport {
  // Diagnostics of related documents. This information is useful
  // in programming languages where code in a file A can generate
  // diagnostics in a file B which A depends on. An example of
  // such a language is C/C++ where marco definitions in a file
  // a.cpp and result in errors in a header file b.hpp.
  //
  // @since 3.17.0
  std::optional<std::unordered_map<std::string_view,
                                   std::variant<FullDocumentDiagnosticReport, UnchangedDocumentDiagnosticReport>>>
      relatedDocuments;

  // A document diagnostic report indicating
  // no changes to the last result. A server can
  // only return `unchanged` if result ids are
  // provided.
  const std::string_view kind;

  // A result id which will be sent on the next
  // diagnostic request for the same document.
  std::string_view resultId;
};

/**
 * A full document diagnostic report for a workspace diagnostic result.
 *
 * @since 3.17.0
 */
struct WorkspaceFullDocumentDiagnosticReport {
  // The URI for which diagnostic information is reported.
  std::string_view uri;

  // The version number for which the diagnostics are reported.
  // If the document is not marked as open `null` can be provided.
  std::int32_t version;

  // A full document diagnostic report.
  const std::string_view kind;

  // An optional result id. If provided it will
  // be sent on the next diagnostic request for the
  // same document.
  std::optional<std::string_view> resultId;

  // The actual items.
  std::vector<Diagnostic> items;
};

/**
 * The parameters of a {@link CodeActionRequest}.
 */
struct CodeActionParams {
  // The document in which the command was invoked.
  TextDocumentIdentifier textDocument;

  // The range for which the command was invoked.
  Range range;

  // Context carrying additional information.
  CodeActionContext context;

  // An optional token that a server can use to report work done progress.
  std::optional<ProgressToken> workDoneToken;

  // An optional token that a server can use to report partial results (e.g. streaming) to
  // the client.
  std::optional<ProgressToken> partialResultToken;
};

/**
 * A code action represents a change that can be performed in code, e.g. to fix a problem or
 * to refactor code.
 *
 * A CodeAction must set either `edit` and/or a `command`. If both are supplied, the `edit` is applied first, then the
 * `command` is executed.
 */
struct CodeAction {
  // A short, human-readable, title for this code action.
  std::string_view title;

  // The kind of the code action.
  //
  // Used to filter code actions.
  std::optional<CodeActionKind> kind;

  // The diagnostics that this code action resolves.
  std::optional<std::vector<Diagnostic>> diagnostics;

  // Marks this as a preferred action. Preferred actions are used by the `auto fix` command and can be targeted
  // by keybindings.
  //
  // A quick fix should be marked preferred if it properly addresses the underlying error.
  // A refactoring should be marked preferred if it is the most reasonable choice of actions to take.
  //
  // @since 3.15.0
  std::optional<bool> isPreferred;

  // Marks that the code action cannot currently be applied.
  //
  // Clients should follow the following guidelines regarding disabled code actions:
  //
  //   - Disabled code actions are not shown in automatic
  //   [lightbulbs](https://code.visualstudio.com/docs/editor/editingevolved#_code-action)
  //     code action menus.
  //
  //   - Disabled actions are shown as faded out in the code action menu when the user requests a more specific type
  //     of code action, such as refactorings.
  //
  //   - If the user has a
  //   [keybinding](https://code.visualstudio.com/docs/editor/refactoring#_keybindings-for-code-actions)
  //     that auto applies a code action and only disabled code actions are returned, the client should show the user an
  //     error message with `reason` in the editor.
  //
  // @since 3.16.0
  std::optional<CodeActionDisabled> disabled;

  // The workspace edit this code action performs.
  std::optional<WorkspaceEdit> edit;

  // A command this code action executes. If a code action
  // provides an edit and a command, first the edit is
  // executed and then the command.
  std::optional<Command> command;

  // A data entry field that is preserved on a code action between
  // a `textDocument/codeAction` and a `codeAction/resolve` request.
  //
  // @since 3.16.0
  std::optional<LSPAny> data;

  // Tags for this code action.
  //
  // @since 3.18.0 - proposed
  std::optional<std::vector<CodeActionTag>> tags;
};

/**
 * The parameters passed via an apply workspace edit request.
 */
struct ApplyWorkspaceEditParams {
  // An optional label of the workspace edit. This label is
  // presented in the user interface for example on an undo
  // stack to undo the workspace edit.
  std::optional<std::string_view> label;

  // The edits to apply.
  WorkspaceEdit edit;

  // Additional data about the edit.
  //
  // @since 3.18.0
  // @proposed
  std::optional<WorkspaceEditMetadata> metadata;
};

/**
 * A change event for a notebook document.
 *
 * @since 3.17.0
 */
struct NotebookDocumentChangeEvent {
  // The changed meta data if any.
  //
  // Note: should always be an object literal (e.g. LSPObject)
  std::optional<LSPObject> metadata;

  // Changes to cells
  std::optional<NotebookDocumentCellChanges> cells;
};

struct InitializeParams {
  // The process Id of the parent process that started
  // the server.
  //
  // Is `null` if the process has not been started by another process.
  // If the parent process is not alive then the server should exit.
  std::int32_t processId;

  // Information about the client
  //
  // @since 3.15.0
  std::optional<ClientInfo> clientInfo;

  // The locale the client is currently showing the user interface
  // in. This must not necessarily be the locale of the operating
  // system.
  //
  // Uses IETF language tags as the value's syntax
  // (See https://en.wikipedia.org/wiki/IETF_language_tag)
  //
  // @since 3.16.0
  std::optional<std::string_view> locale;

  // The rootPath of the workspace. Is null
  // if no folder is open.
  //
  // @deprecated in favour of rootUri.
  std::optional<std::string_view> rootPath;

  // The rootUri of the workspace. Is null if no
  // folder is open. If both `rootPath` and `rootUri` are set
  // `rootUri` wins.
  //
  // @deprecated in favour of workspaceFolders.
  std::string_view rootUri;

  // The capabilities provided by the client (editor or tool)
  ClientCapabilities capabilities;

  // User provided initialization options.
  std::optional<LSPAny> initializationOptions;

  // The initial trace setting. If omitted trace is disabled ('off').
  std::optional<TraceValue> trace;

  // The workspace folders configured in the client when the server starts.
  //
  // This property is only available if the client supports workspace folders.
  // It can be `null` if the client supports workspace folders but none are
  // configured.
  //
  // @since 3.6.0
  std::optional<std::vector<WorkspaceFolder>> workspaceFolders;
};

/**
 * Registration options specific to a notebook.
 *
 * @since 3.17.0
 */
struct NotebookDocumentSyncRegistrationOptions {
  // The notebooks to be synced
  std::vector<std::variant<NotebookDocumentFilterWithNotebook, NotebookDocumentFilterWithCells>> notebookSelector;

  // Whether save notification should be forwarded to
  // the server. Will only be honored if mode === `notebook`.
  std::optional<bool> save;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

/**
 * A document selector is the combination of one or many document filters.
 *
 * @sample `let sel:DocumentSelector = [{ language: 'typescript' }, { language: 'json', pattern: '**∕tsconfig.json' }]`;
 *
 * The use of a string as a document filter is deprecated @since 3.16.0.
 */
using DocumentSelector = std::vector<DocumentFilter>;

/**
 * The result of a document diagnostic pull request. A report can
 * either be a full report containing all diagnostics for the
 * requested document or an unchanged report indicating that nothing
 * has changed in terms of diagnostics in comparison to the last
 * pull request.
 *
 * @since 3.17.0
 */
using DocumentDiagnosticReport =
    std::variant<RelatedFullDocumentDiagnosticReport, RelatedUnchangedDocumentDiagnosticReport>;

/**
 * A workspace diagnostic document report.
 *
 * @since 3.17.0
 */
using WorkspaceDocumentDiagnosticReport =
    std::variant<WorkspaceFullDocumentDiagnosticReport, WorkspaceUnchangedDocumentDiagnosticReport>;

/**
 * The params sent in a change notebook document notification.
 *
 * @since 3.17.0
 */
struct DidChangeNotebookDocumentParams {
  // The notebook document that did change. The version number points
  // to the version after all provided changes have been applied. If
  // only the text document content of a cell changes the notebook version
  // doesn't necessarily have to change.
  VersionedNotebookDocumentIdentifier notebookDocument;

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
  NotebookDocumentChangeEvent change;
};

/**
 * General text document registration options.
 */
struct TextDocumentRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;
};

/**
 * A workspace diagnostic report.
 *
 * @since 3.17.0
 */
struct WorkspaceDiagnosticReport {
  std::vector<WorkspaceDocumentDiagnosticReport> items;
};

/**
 * A partial result for a workspace diagnostic report.
 *
 * @since 3.17.0
 */
struct WorkspaceDiagnosticReportPartialResult {
  std::vector<WorkspaceDocumentDiagnosticReport> items;
};

struct ImplementationRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

struct TypeDefinitionRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

struct DocumentColorRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

struct FoldingRangeRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

struct DeclarationRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

struct SelectionRangeRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

/**
 * Call hierarchy options used during static or dynamic registration.
 *
 * @since 3.16.0
 */
struct CallHierarchyRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

/**
 * @since 3.16.0
 */
struct SemanticTokensRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // The legend used by the server
  SemanticTokensLegend legend;

  // Server supports providing semantic tokens for a specific range
  // of a document.
  std::optional<std::variant<bool, LiteralStub>> range;

  // Server supports providing semantic tokens for a full document.
  std::optional<std::variant<bool, SemanticTokensFullDelta>> full;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

struct LinkedEditingRangeRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

struct MonikerRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;
};

/**
 * Type hierarchy options used during static or dynamic registration.
 *
 * @since 3.17.0
 */
struct TypeHierarchyRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

/**
 * Inline value options used during static or dynamic registration.
 *
 * @since 3.17.0
 */
struct InlineValueRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

/**
 * Inlay hint options used during static or dynamic registration.
 *
 * @since 3.17.0
 */
struct InlayHintRegistrationOptions {
  // The server provides support to resolve additional
  // information for an inlay hint item.
  std::optional<bool> resolveProvider;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

/**
 * Diagnostic registration options.
 *
 * @since 3.17.0
 */
struct DiagnosticRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // An optional identifier under which the diagnostics are
  // managed by the client.
  std::optional<std::string_view> identifier;

  // Whether the language has inter file dependencies meaning that
  // editing code in one file can result in a different diagnostic
  // set in another file. Inter file dependencies are common for
  // most programming languages and typically uncommon for linters.
  bool interFileDependencies;

  // The server provides support for workspace diagnostics as well.
  bool workspaceDiagnostics;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

/**
 * Inline completion options used during static or dynamic registration.
 *
 * @since 3.18.0
 * @proposed
 */
struct InlineCompletionRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // The id used to register the request. The id can be used to deregister
  // the request again. See also Registration#id.
  std::optional<std::string_view> id;
};

/**
 * Describe options to be used when registered for text document change events.
 */
struct TextDocumentChangeRegistrationOptions {
  // How documents are synced to the server.
  TextDocumentSyncKind syncKind;

  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;
};

/**
 * Save registration options.
 */
struct TextDocumentSaveRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // The client is supposed to include the content on save.
  std::optional<bool> includeText;
};

/**
 * Registration options for a {@link CompletionRequest}.
 */
struct CompletionRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // Most tools trigger completion request automatically without explicitly requesting
  // it using a keyboard shortcut (e.g. Ctrl+Space). Typically they do so when the user
  // starts to type an identifier. For example if the user types `c` in a JavaScript file
  // code complete will automatically pop up present `console` besides others as a
  // completion item. Characters that make up identifiers don't need to be listed here.
  //
  // If code complete should automatically be trigger on characters not being valid inside
  // an identifier (for example `.` in JavaScript) list them in `triggerCharacters`.
  std::optional<std::vector<std::string_view>> triggerCharacters;

  // The list of all possible characters that commit a completion. This field can be used
  // if clients don't support individual commit characters per completion item. See
  // `ClientCapabilities.textDocument.completion.completionItem.commitCharactersSupport`
  //
  // If a server provides both `allCommitCharacters` and commit characters on an individual
  // completion item the ones on the completion item win.
  //
  // @since 3.2.0
  std::optional<std::vector<std::string_view>> allCommitCharacters;

  // The server provides support to resolve additional
  // information for a completion item.
  std::optional<bool> resolveProvider;

  // The server supports the following `CompletionItem` specific
  // capabilities.
  //
  // @since 3.17.0
  std::optional<ServerCompletionItemOptions> completionItem;
};

/**
 * Registration options for a {@link HoverRequest}.
 */
struct HoverRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;
};

/**
 * Registration options for a {@link SignatureHelpRequest}.
 */
struct SignatureHelpRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // List of characters that trigger signature help automatically.
  std::optional<std::vector<std::string_view>> triggerCharacters;

  // List of characters that re-trigger signature help.
  //
  // These trigger characters are only active when signature help is already showing. All trigger characters
  // are also counted as re-trigger characters.
  //
  // @since 3.15.0
  std::optional<std::vector<std::string_view>> retriggerCharacters;
};

/**
 * Registration options for a {@link DefinitionRequest}.
 */
struct DefinitionRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;
};

/**
 * Registration options for a {@link ReferencesRequest}.
 */
struct ReferenceRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;
};

/**
 * Registration options for a {@link DocumentHighlightRequest}.
 */
struct DocumentHighlightRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;
};

/**
 * Registration options for a {@link DocumentSymbolRequest}.
 */
struct DocumentSymbolRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // A human-readable string that is shown when multiple outlines trees
  // are shown for the same document.
  //
  // @since 3.16.0
  std::optional<std::string_view> label;
};

/**
 * Registration options for a {@link CodeActionRequest}.
 */
struct CodeActionRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // CodeActionKinds that this server may return.
  //
  // The list of kinds may be generic, such as `CodeActionKind.Refactor`, or the server
  // may list out every specific kind they provide.
  std::optional<std::vector<CodeActionKind>> codeActionKinds;

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
  std::optional<std::vector<CodeActionKindDocumentation>> documentation;

  // The server provides support to resolve additional
  // information for a code action.
  //
  // @since 3.16.0
  std::optional<bool> resolveProvider;
};

/**
 * Registration options for a {@link CodeLensRequest}.
 */
struct CodeLensRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // Code lens has a resolve provider as well.
  std::optional<bool> resolveProvider;
};

/**
 * Registration options for a {@link DocumentLinkRequest}.
 */
struct DocumentLinkRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // Document links have a resolve provider as well.
  std::optional<bool> resolveProvider;
};

/**
 * Registration options for a {@link DocumentFormattingRequest}.
 */
struct DocumentFormattingRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;
};

/**
 * Registration options for a {@link DocumentRangeFormattingRequest}.
 */
struct DocumentRangeFormattingRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // Whether the server supports formatting multiple ranges at once.
  //
  // @since 3.18.0
  // @proposed
  std::optional<bool> rangesSupport;
};

/**
 * Registration options for a {@link DocumentOnTypeFormattingRequest}.
 */
struct DocumentOnTypeFormattingRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // A character on which formatting should be triggered, like `{`.
  std::string_view firstTriggerCharacter;

  // More trigger characters.
  std::optional<std::vector<std::string_view>> moreTriggerCharacter;
};

/**
 * Registration options for a {@link RenameRequest}.
 */
struct RenameRegistrationOptions {
  // A document selector to identify the scope of the registration. If set to null
  // the document selector provided on the client side will be used.
  DocumentSelector documentSelector;

  // Renames should be checked and tested before being executed.
  //
  // @since version 3.12.0
  std::optional<bool> prepareProvider;
};

/**
 * Defines the capabilities provided by a language
 * server.
 */
struct ServerCapabilities {
  // The position encoding the server picked from the encodings offered
  // by the client via the client capability `general.positionEncodings`.
  //
  // If the client didn't provide any position encodings the only valid
  // value that a server can return is 'utf-16'.
  //
  // If omitted it defaults to 'utf-16'.
  //
  // @since 3.17.0
  std::optional<PositionEncodingKind> positionEncoding;

  // Defines how text documents are synced. Is either a detailed structure
  // defining each notification or for backwards compatibility the
  // TextDocumentSyncKind number.
  std::optional<std::variant<TextDocumentSyncOptions, TextDocumentSyncKind>> textDocumentSync;

  // Defines how notebook documents are synced.
  //
  // @since 3.17.0
  std::optional<std::variant<NotebookDocumentSyncOptions, NotebookDocumentSyncRegistrationOptions>>
      notebookDocumentSync;

  // The server provides completion support.
  std::optional<CompletionOptions> completionProvider;

  // The server provides hover support.
  std::optional<std::variant<bool, HoverOptions>> hoverProvider;

  // The server provides signature help support.
  std::optional<SignatureHelpOptions> signatureHelpProvider;

  // The server provides Goto Declaration support.
  std::optional<std::variant<bool, DeclarationOptions, DeclarationRegistrationOptions>> declarationProvider;

  // The server provides goto definition support.
  std::optional<std::variant<bool, DefinitionOptions>> definitionProvider;

  // The server provides Goto Type Definition support.
  std::optional<std::variant<bool, TypeDefinitionOptions, TypeDefinitionRegistrationOptions>> typeDefinitionProvider;

  // The server provides Goto Implementation support.
  std::optional<std::variant<bool, ImplementationOptions, ImplementationRegistrationOptions>> implementationProvider;

  // The server provides find references support.
  std::optional<std::variant<bool, ReferenceOptions>> referencesProvider;

  // The server provides document highlight support.
  std::optional<std::variant<bool, DocumentHighlightOptions>> documentHighlightProvider;

  // The server provides document symbol support.
  std::optional<std::variant<bool, DocumentSymbolOptions>> documentSymbolProvider;

  // The server provides code actions. CodeActionOptions may only be
  // specified if the client states that it supports
  // `codeActionLiteralSupport` in its initial `initialize` request.
  std::optional<std::variant<bool, CodeActionOptions>> codeActionProvider;

  // The server provides code lens.
  std::optional<CodeLensOptions> codeLensProvider;

  // The server provides document link support.
  std::optional<DocumentLinkOptions> documentLinkProvider;

  // The server provides color provider support.
  std::optional<std::variant<bool, DocumentColorOptions, DocumentColorRegistrationOptions>> colorProvider;

  // The server provides workspace symbol support.
  std::optional<std::variant<bool, WorkspaceSymbolOptions>> workspaceSymbolProvider;

  // The server provides document formatting.
  std::optional<std::variant<bool, DocumentFormattingOptions>> documentFormattingProvider;

  // The server provides document range formatting.
  std::optional<std::variant<bool, DocumentRangeFormattingOptions>> documentRangeFormattingProvider;

  // The server provides document formatting on typing.
  std::optional<DocumentOnTypeFormattingOptions> documentOnTypeFormattingProvider;

  // The server provides rename support. RenameOptions may only be
  // specified if the client states that it supports
  // `prepareSupport` in its initial `initialize` request.
  std::optional<std::variant<bool, RenameOptions>> renameProvider;

  // The server provides folding provider support.
  std::optional<std::variant<bool, FoldingRangeOptions, FoldingRangeRegistrationOptions>> foldingRangeProvider;

  // The server provides selection range support.
  std::optional<std::variant<bool, SelectionRangeOptions, SelectionRangeRegistrationOptions>> selectionRangeProvider;

  // The server provides execute command support.
  std::optional<ExecuteCommandOptions> executeCommandProvider;

  // The server provides call hierarchy support.
  //
  // @since 3.16.0
  std::optional<std::variant<bool, CallHierarchyOptions, CallHierarchyRegistrationOptions>> callHierarchyProvider;

  // The server provides linked editing range support.
  //
  // @since 3.16.0
  std::optional<std::variant<bool, LinkedEditingRangeOptions, LinkedEditingRangeRegistrationOptions>>
      linkedEditingRangeProvider;

  // The server provides semantic tokens support.
  //
  // @since 3.16.0
  std::optional<std::variant<SemanticTokensOptions, SemanticTokensRegistrationOptions>> semanticTokensProvider;

  // The server provides moniker support.
  //
  // @since 3.16.0
  std::optional<std::variant<bool, MonikerOptions, MonikerRegistrationOptions>> monikerProvider;

  // The server provides type hierarchy support.
  //
  // @since 3.17.0
  std::optional<std::variant<bool, TypeHierarchyOptions, TypeHierarchyRegistrationOptions>> typeHierarchyProvider;

  // The server provides inline values.
  //
  // @since 3.17.0
  std::optional<std::variant<bool, InlineValueOptions, InlineValueRegistrationOptions>> inlineValueProvider;

  // The server provides inlay hints.
  //
  // @since 3.17.0
  std::optional<std::variant<bool, InlayHintOptions, InlayHintRegistrationOptions>> inlayHintProvider;

  // The server has support for pull model diagnostics.
  //
  // @since 3.17.0
  std::optional<std::variant<DiagnosticOptions, DiagnosticRegistrationOptions>> diagnosticProvider;

  // Inline completion options used during static registration.
  //
  // @since 3.18.0
  // @proposed
  std::optional<std::variant<bool, InlineCompletionOptions>> inlineCompletionProvider;

  // Workspace specific server capabilities.
  std::optional<WorkspaceOptions> workspace;

  // Experimental server capabilities.
  std::optional<LSPAny> experimental;
};

/**
 * The result returned from an initialize request.
 */
struct InitializeResult {
  // The capabilities the language server provides.
  ServerCapabilities capabilities;

  // Information about the server.
  //
  // @since 3.15.0
  std::optional<ServerInfo> serverInfo;
};

}  // namespace lsp

template <>
struct glz::meta<lsp::SemanticTokenTypes> {
  using enum lsp::SemanticTokenTypes;
  static constexpr auto value = enumerate(
      "namespace", kNamespace, "type", kType, "class", kClass, "enum", kEnum, "interface", kInterface, "struct",
      kStruct, "typeParameter", kTypeparameter, "parameter", kParameter, "variable", kVariable, "property", kProperty,
      "enumMember", kEnummember, "event", kEvent, "function", kFunction, "method", kMethod, "macro", kMacro, "keyword",
      kKeyword, "modifier", kModifier, "comment", kComment, "string", kString, "number", kNumber, "regexp", kRegexp,
      "operator", kOperator, "decorator", kDecorator, "label", kLabel);
};

template <>
struct glz::meta<lsp::SemanticTokenModifiers> {
  using enum lsp::SemanticTokenModifiers;
  static constexpr auto value =
      enumerate("declaration", kDeclaration, "definition", kDefinition, "readonly", kReadonly, "static", kStatic,
                "deprecated", kDeprecated, "abstract", kAbstract, "async", kAsync, "modification", kModification,
                "documentation", kDocumentation, "defaultLibrary", kDefaultlibrary);
};

template <>
struct glz::meta<lsp::DocumentDiagnosticReportKind> {
  using enum lsp::DocumentDiagnosticReportKind;
  static constexpr auto value = enumerate("full", kFull, "unchanged", kUnchanged);
};

template <>
struct glz::meta<lsp::FoldingRangeKind> {
  using enum lsp::FoldingRangeKind;
  static constexpr auto value = enumerate("comment", kComment, "imports", kImports, "region", kRegion);
};

template <>
struct glz::meta<lsp::UniquenessLevel> {
  using enum lsp::UniquenessLevel;
  static constexpr auto value =
      enumerate("document", kDocument, "project", kProject, "group", kGroup, "scheme", kScheme, "global", kGlobal);
};

template <>
struct glz::meta<lsp::MonikerKind> {
  using enum lsp::MonikerKind;
  static constexpr auto value = enumerate("import", kImport, "export", kExport, "local", kLocal);
};

template <>
struct glz::meta<lsp::CodeActionKind> {
  using enum lsp::CodeActionKind;
  static constexpr auto value = enumerate(
      "", kEmpty, "quickfix", kQuickfix, "refactor", kRefactor, "refactor.extract", kRefactorextract, "refactor.inline",
      kRefactorinline, "refactor.move", kRefactormove, "refactor.rewrite", kRefactorrewrite, "source", kSource,
      "source.organizeImports", kSourceorganizeimports, "source.fixAll", kSourcefixall, "notebook", kNotebook);
};

template <>
struct glz::meta<lsp::TraceValue> {
  using enum lsp::TraceValue;
  static constexpr auto value = enumerate("off", kOff, "messages", kMessages, "verbose", kVerbose);
};

template <>
struct glz::meta<lsp::MarkupKind> {
  using enum lsp::MarkupKind;
  static constexpr auto value = enumerate("plaintext", kPlaintext, "markdown", kMarkdown);
};

template <>
struct glz::meta<lsp::PositionEncodingKind> {
  using enum lsp::PositionEncodingKind;
  static constexpr auto value = enumerate("utf-8", kUtf8, "utf-16", kUtf16, "utf-32", kUtf32);
};

template <>
struct glz::meta<lsp::FileOperationPatternKind> {
  using enum lsp::FileOperationPatternKind;
  static constexpr auto value = enumerate("file", kFile, "folder", kFolder);
};

template <>
struct glz::meta<lsp::ResourceOperationKind> {
  using enum lsp::ResourceOperationKind;
  static constexpr auto value = enumerate("create", kCreate, "rename", kRename, "delete", kDelete);
};

template <>
struct glz::meta<lsp::FailureHandlingKind> {
  using enum lsp::FailureHandlingKind;
  static constexpr auto value = enumerate("abort", kAbort, "transactional", kTransactional, "textOnlyTransactional",
                                          kTextonlytransactional, "undo", kUndo);
};

template <>
struct glz::meta<lsp::TokenFormat> {
  using enum lsp::TokenFormat;
  static constexpr auto value = enumerate("relative", kRelative);
};

// NOLINTEND(readability-identifier-naming)

#include "LSProtocolEx.h"
