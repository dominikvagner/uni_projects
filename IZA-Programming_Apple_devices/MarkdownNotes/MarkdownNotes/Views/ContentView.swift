import SwiftUI

struct ContentView: View {
    @Binding var document: MyDocument
    @State private var outputSheet: Bool = false
    @State private var helpSheet: Bool = false
    @State private var copyAlert: Bool = false
    
    var body: some View {
        NavigationView {
            TextEditor(text: $document.content)
                .disableAutocorrection(true)
                .textInputAutocapitalization(.never)
                .toolbar {
                    ToolbarItemGroup(placement: .keyboard) {
                        Spacer()
                        Button { document.content += "** **"
                        } label: { Image(systemName: "bold") }
                        
                        Button { document.content += "* *"
                        } label: { Image(systemName: "italic") }
                        
                        Button { document.content += "~~ ~~"
                        } label: { Image(systemName: "strikethrough") }
                        
                        Button { document.content += "` `"
                        } label: { Image(systemName: "terminal") }
                        
                        Button { document.content += "\n- "
                        } label: { Image(systemName: "list.bullet") }
                        
                        Button { document.content += "\n1. "
                        } label: { Image(systemName: "list.number") }
                        
                        Button { document.content += "[ ]( ) "
                        } label: { Image(systemName: "link") }
                        Spacer()
                    }
                }
        }
        .navigationTitle("Markdown 📝")
        .toolbar {
            ToolbarItemGroup(placement: .navigationBarTrailing) {
            Button { helpSheet.toggle()
                } label: { Image(systemName: "questionmark") }
                
                Button { UIPasteboard.general.string = document.content
                    copyAlert.toggle()
                } label: { Image(systemName: "doc.on.clipboard") }
                .alert(isPresented: $copyAlert) {
                    Alert(title: Text("Document contents copied into clipboard."))
                }
                
                Button { outputSheet.toggle()
                } label: { Image(systemName: "eye") }
            }
        }
        .sheet(isPresented: $outputSheet) {
            OutputView(document: $document)
        }
        .sheet(isPresented: $helpSheet) {
            HelpView()
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView(document: .constant(MyDocument()))
    }
}
