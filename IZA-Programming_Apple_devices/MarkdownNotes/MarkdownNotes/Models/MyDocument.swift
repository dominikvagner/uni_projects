import SwiftUI
import UniformTypeIdentifiers

struct MyDocument: FileDocument {
    static var readableContentTypes: [UTType] {[.plainText]}
    static var writableContentTypes: [UTType] {[.plainText]}
    
    var content = ""
    
    init(initialText: String = "Something...") {
        content = initialText
    }
    
    init(configuration: FileDocumentReadConfiguration) throws {
        if let data = configuration.file.regularFileContents {
            content = String(decoding: data, as: UTF8.self)
        }
    }
    
    func fileWrapper(configuration: FileDocumentWriteConfiguration) throws -> FileWrapper {
        let data = Data(content.utf8)
        return FileWrapper(regularFileWithContents: data)
    }
    
    func getAttributedString() -> AttributedString {
        guard let attributedString = try? AttributedString(styledMarkdown: content
        ) else {
            return AttributedString(content)
        }
        return attributedString
    }
}
