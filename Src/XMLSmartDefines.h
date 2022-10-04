#pragma once

#define RETURN_ATTR_DEFAULT_VALUE(attr,default)   OleVariant AVariant = TXMLNode::AttributeNodes->FindNode(attr) ? TXMLNode::AttributeNodes->Nodes[UnicodeString(attr)]->NodeValue : default; return AVariant;
#define RETURN_ATTR_DEFAULT_TEXT(attr,default)   OleVariant AVariant = TXMLNode::AttributeNodes->FindNode(attr) ? TXMLNode::AttributeNodes->Nodes[UnicodeString(attr)]->Text : default; return AVariant;

