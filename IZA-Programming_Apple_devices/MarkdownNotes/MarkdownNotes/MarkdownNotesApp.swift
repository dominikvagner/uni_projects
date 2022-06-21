import SwiftUI

@main
struct MarkdownNotes: App {
    var body: some Scene {
        DocumentGroup(newDocument: MyDocument()) { file in
            ContentView(document: file.$document)
        }
    }
}

