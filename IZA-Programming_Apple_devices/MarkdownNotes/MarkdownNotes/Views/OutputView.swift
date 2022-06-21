import SwiftUI

struct OutputView: View {
    @Binding var document: MyDocument
    
    var body: some View {
        ScrollView {
            Text(document.getAttributedString())
                .frame(maxWidth: .infinity, alignment: .topLeading)
                .padding()
                .textSelection(.enabled)
        }
    }
}

struct OutputView_Previews: PreviewProvider {
    static var previews: some View {
        OutputView(document: .constant(MyDocument()))
    }
}
