import SwiftUI
import Foundation
import UIKit

fileprivate enum MarkdownStyledBlock: Equatable {
    case generic
    case headline(Int)
    case paragraph
    case unorderedListElement
    case orderedListElement(Int)
    case blockquote
    case code(String?)
}

extension AttributedString {
    init(styledMarkdown content: String, fontSize: CGFloat = UIFont.preferredFont(forTextStyle: .body).pointSize) throws {
        var attrString = try AttributedString(
            markdown: content,
            options: .init(
                allowsExtendedAttributes: true,
                interpretedSyntax: .full,
                failurePolicy: .returnPartiallyParsedIfPossible
            ),
            baseURL: nil
        )
        
        attrString.foregroundColor = .label
        
        let inline: [InlinePresentationIntent] = [.emphasized, .stronglyEmphasized, .code, .strikethrough]
        
        for inline in inline {
            var sourceAttrContainer = AttributeContainer()
            sourceAttrContainer.inlinePresentationIntent = inline
            
            var targetAttrContainer = AttributeContainer()
            
            switch inline {
                
            case .emphasized:
                targetAttrContainer.font = .italicSystemFont(ofSize: fontSize)
                
            case .stronglyEmphasized:
                targetAttrContainer.font = .boldSystemFont(ofSize: fontSize)
                
            case .code:
                targetAttrContainer.font = .monospacedSystemFont(ofSize: fontSize, weight: .regular)
                targetAttrContainer.foregroundColor = .codeForeground
                targetAttrContainer.backgroundColor = .codeBackground
                
            case .strikethrough:
                targetAttrContainer.strikethroughStyle = .single
                
            default:
                break
            }
            
            attrString = attrString.replacingAttributes(sourceAttrContainer, with: targetAttrContainer)
        }
        
        var previousListID = 0
        for (intentBlock, intentRange) in attrString.runs[AttributeScopes.FoundationAttributes.PresentationIntentAttribute.self].reversed() {
            guard let intentBlock = intentBlock else {continue}
            
            var block: MarkdownStyledBlock = .generic
            var currentElementOrdinal: Int = 0
            var currentListID: Int = 0
            
            for intent in intentBlock.components {
                switch intent.kind {
                case .paragraph:
                    if block == .generic { block = .paragraph }
                case .header(level: let level):
                    block = .headline(level)
                case .orderedList:
                    block = .orderedListElement(currentElementOrdinal)
                    currentListID = intent.identity
                case .unorderedList:
                    block = .unorderedListElement
                    currentListID = intent.identity
                case .listItem(ordinal: let ordinal):
                    currentElementOrdinal = ordinal
                    if block != .unorderedListElement {
                        block = .orderedListElement(ordinal)
                    }
                case .codeBlock(languageHint: let languageHint):
                    block = .code(languageHint)
                case .blockQuote:
                    block = .blockquote
                default:
                    break
                }
            }
            
            switch block {
            case .generic:
                assertionFailure(intentBlock.debugDescription)
            case .headline(let level):
                switch level {
                case 1:
                    attrString[intentRange].font = .largeTitle
                case 2:
                    attrString[intentRange].font = .title
                case 3:
                    attrString[intentRange].font = .title2
                case 4:
                    attrString[intentRange].font = .title3
                case 5:
                    attrString[intentRange].font = .headline
                case 6:
                    attrString[intentRange].font = .subheadline
                default:
                    break
                }
            case .paragraph:
                break
            case .unorderedListElement:
                attrString.characters.insert(contentsOf: "•\t", at: intentRange.lowerBound)
                attrString[intentRange].paragraphStyle = previousListID == currentListID ? listParagraphStyle : endOfListParagraphStyle
            case .orderedListElement(let ordinal):
                attrString.characters.insert(contentsOf: "\(ordinal).\t", at: intentRange.lowerBound)
                attrString[intentRange].paragraphStyle = previousListID == currentListID ? listParagraphStyle : endOfListParagraphStyle
            case .blockquote:
                attrString[intentRange].paragraphStyle = defaultParagraphStyle
                attrString[intentRange].foregroundColor = .secondary
            case .code:
                attrString[intentRange].paragraphStyle = codeParagraphStyle
                attrString[intentRange].font = .monospacedSystemFont(
                    ofSize: fontSize,
                    weight: .regular
                )
            }
            
            previousListID = currentListID
            
            if intentRange.lowerBound != attrString.startIndex {
                attrString.characters.insert(contentsOf: "\n", at: intentRange.lowerBound)
            }
        }
        
        self = attrString
    }
}

fileprivate let defaultParagraphStyle: NSParagraphStyle = {
    var paragraphStyle = NSMutableParagraphStyle()
    paragraphStyle.paragraphSpacing = 10.0
    paragraphStyle.minimumLineHeight = 20.0
    return paragraphStyle
}()

fileprivate let listParagraphStyle: NSMutableParagraphStyle = {
    var paragraphStyle = NSMutableParagraphStyle()
    paragraphStyle.addTabStop(NSTextTab(textAlignment: .left, location: 20))
    paragraphStyle.headIndent = 20
    paragraphStyle.minimumLineHeight = 20.0
    return paragraphStyle
}()

fileprivate let endOfListParagraphStyle: NSMutableParagraphStyle = {
    var paragraphStyle = NSMutableParagraphStyle()
    paragraphStyle.addTabStop(NSTextTab(textAlignment: .left, location: 20))
    paragraphStyle.headIndent = 20
    paragraphStyle.minimumLineHeight = 20.0
    paragraphStyle.paragraphSpacing = 20.0
    return paragraphStyle
}()

fileprivate let codeParagraphStyle: NSParagraphStyle = {
    var paragraphStyle = NSMutableParagraphStyle()
    paragraphStyle.minimumLineHeight = 20.0
    paragraphStyle.headIndent = 20
    paragraphStyle.firstLineHeadIndent = 20
    return paragraphStyle
}()

extension UIColor {
    static let codeBackground = UIColor(red: 43.0/255.0, green: 20.0/255.0, blue: 26.0/255.0, alpha: 1)
    static let codeForeground = UIColor(red: 221.0/255.0, green: 73.0/255.0, blue: 110.0/255.0, alpha: 1)
}
