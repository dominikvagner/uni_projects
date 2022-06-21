import SwiftUI

struct HelpView: View {
    @State var document: MyDocument = MyDocument(initialText:
        """
        Headings:
        # Heading level 1
        ## Heading level 2
        ### Heading level 3
        #### Heading level 4
        ##### Heading level 5
        ###### Heading level 6
          
        Text Emphasis:
        **Bold**
        *Italic*
        ~~Strikethrough~~
          
        Code:
        Something... `Inline Code` Something...
        ```
        Code
        Block
        ```
        
        Lists:
        - Unordered Item 1
        - Unordered Item 2
        
        1. Ordered Item 1
        1. Ordered Item 2
        
        Links:
        For more Markdown syntax help visit
        [Markdown Guide](https://markdownguide.org/basic-syntax)
        """
    )
    
    var body: some View {
        ScrollView {
            VStack {
                HStack {
                    Text("Program Help")
                        .font(.largeTitle)
                        .padding(.all)
                    Spacer()
                }
                VStack {
                    Text(
                        """
                        MarkdownNotes is a simple markdown editor, coded in
                        Swift Playgrounds for iPad.
                        It doesn't yet support all possible markdown syntax,
                        it uses the AttributedString API from SwiftUI and
                        expands a little on the limited markdown support it has.
                        Above your keyboard should also pop-up a custom toolbar
                        for easier inserting of a few supported markdown syntax.
                        """ )
                        .frame(maxWidth: .infinity, alignment: .topLeading)
                        .padding()
                    Text("Here is what it can do:")
                        .frame(maxWidth: .infinity, alignment: .topLeading)
                        .padding()
                        .font(.title2)
                }
                HStack {
                    Text(document.content)
                        .padding()
                        .textSelection(.enabled)
                    Spacer()
                }
                .overlay(
                    RoundedRectangle(cornerRadius: 8)
                        .stroke(.secondary, lineWidth: 1)
                )
                .padding(.all)
                HStack {
                    Text("Rendered Output:")
                        .padding(.all)
                        .textSelection(.enabled)
                        .font(.title2)
                    Spacer()
                }
                Text(document.getAttributedString())
                    .frame(maxWidth: .infinity, alignment: .topLeading)
                    .textSelection(.enabled)
                    .padding()
                    .overlay(
                        RoundedRectangle(cornerRadius: 8)
                            .stroke(.secondary, lineWidth: 1)
                    )
                    .padding(.all)
                Spacer()
            }
        }
    }
}

struct HelpView_Previews: PreviewProvider {
    static var previews: some View {
        HelpView()
    }
}
