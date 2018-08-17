/*
 *  dom.hpp
 *  Ubit GUI Toolkit - Version 8
 *  (C) 2018 Chris Daley
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#ifndef UBIT_CORE_DOM_H_
#define UBIT_CORE_DOM_H_

#include <map>
#include <ubit/udoc.hpp>
#include <ubit/core/uclassImpl.hpp>  //StyleSheet
#include <ubit/ustr.hpp>
#include <fstream>

namespace ubit {
  
  class XmlGrammar;
  class XmlGrammars;
   
    /** XML Comment.
   */
  class Comment : public Node {
  public:
    UCLASSDEF("comment", Comment, new Comment)

    Comment(const String& = "");  
    virtual int getNodeType() const {return COMMENT_NODE;}
    virtual const String& getNodeName() const;
    virtual String getNodeValue() const {return *data;}
    virtual const String& getData() const {return *data;}
    
  private:
    uptr<String> data;
  };
  
    /** XML CDATASection.
   */
  class CDATASection : public Node { // pas bon: va afficher cdata
  public:
    UCLASSDEF("CDATASection", CDATASection, new CDATASection)
    
    CDATASection(const String& = "");
    virtual int getNodeType() const {return CDATA_SECTION_NODE;}
    virtual const String& getNodeName() const;
    virtual String getNodeValue() const {return *data;}
    virtual const String& getData() const {return *data;}
    
  private:
    uptr<String> data;
  };
  
    /** XML ProcessingInstruction.
   */
  class ProcessingInstruction : public Node {
  public:
    UCLASSDEF("processinginstruction", ProcessingInstruction, new ProcessingInstruction("",""))
    
    ProcessingInstruction(const String& target, const String& data);
    virtual int getNodeType() const {return PROCESSING_INSTRUCTION_NODE;}
    virtual const String& getNodeName()  const {return getTarget();}
    virtual String getNodeValue() const {return getData();}
    virtual const String& getTarget() const {return *target;}
    virtual const String& getData() const {return *data;}
    
  private:
    uptr<String> target, data;
  };
  
    /** XML DocType.
   */
  class XmlDocType : public Node {
  public:
    UCLASSDEF("doctype", XmlDocType, new XmlDocType("","",""))
    
    XmlDocType(const String& name, const String& public_id, const String& system_id);
    virtual ~XmlDocType();  
    virtual int getNodeType() const {return DOCUMENT_TYPE_NODE;}
    virtual const String& getNodeName() const {return *name;}
    virtual const String& getName()     const {return *name;}
    virtual const String& getPublicId() const {return *public_id;}
    virtual const String& getSystemId() const {return *system_id;}
    
  private:
    friend class XmlDocument;
    uptr<String> name, public_id, system_id;
    //XmlDocType();
  };
  
  
    /** XML Doc.
   */
  class XmlDocument : public Document {
  public:
    UCLASSDEF("document", XmlDocument, new XmlDocument)
    
    XmlDocument();
    XmlDocument(const String& pathname);
    virtual ~XmlDocument();
    
    virtual int getNodeType() const {return DOCUMENT_NODE;}
    virtual const String& getNodeName() const;
    virtual String getNodeValue() const {return "";}
    
    virtual Element* getDocumentElement() {return doc_elem;}
    virtual const Element* getDocumentElement() const {return doc_elem;}
    virtual const XmlDocType* getDoctype() const {return doc_type;}
    virtual const String& getXmlVersion()  const {return *xml_version;}
    virtual const String& getXmlEncoding() const {return *xml_encoding;}
    virtual bool  isXmlStandalone()      const {return xml_standalone;}
    
    virtual Attribute* createAttribute(const String& name);
    virtual Element* createElement(const String& name);
    virtual String* createTextNode(const String& data);
    virtual Comment* createComment(const String& data);
    virtual CDATASection* createCDATASection(const String& data);
    virtual ProcessingInstruction* createProcessingInstruction(const String& target, const String& data);
    
    virtual void initElement(Element*);
    virtual void setClassStyle(Element*, const String& name, const String& value);
    virtual void setIdStyle(Element*, const String& name, const String& value);
    
    virtual void addGrammar(const XmlGrammar&);
    ///< adds a grammar to the document.
    
    virtual void print(std::ostream& fout);
    ///< prints the XML tree on this stream (eg. std::cout).
    
    virtual void print(String& buffer);
    ///< prints the XML tree on this buffer.
    
    virtual const DocumentAttachments* getAttachments() const;
    ///< returns the list of attachments.
    
    virtual int  loadAttachments(bool reload = false);
    virtual bool loadAttachment(DocumentAttachment*, bool reload = false);
    virtual void addAttachment(DocumentAttachment*);
    ///< impl.
    
    const StyleSheet& getStyleSheet() const {return doc_stylesheet;}
    StyleSheet& getStyleSheet() {return doc_stylesheet;}
    ///< impl.
    
  protected:
    friend class XmlParser;
    static const String NodeName;
    uptr<String> xml_version, xml_encoding;
    bool xml_standalone;
    XmlDocType* doc_type;
    XmlGrammars* grammars;
    StyleSheet doc_stylesheet;
    Element* doc_elem;
    DocumentAttachments attachments;
    void constructs();
    virtual void setClassIdStyle(Element*, const String& name, const String& value);
  };
  
    /** [impl] XML Creator.
   */
  struct XmlCreator : public DocumentCreator {
    XmlCreator();
    virtual Document* create(DocumentSource& doc);
    
    bool load_objects;  ///< load HTML objects (eg. images) [default=true].
    bool keep_comments; ///< keep comments [default=false].    
    bool permissive;    ///< parse documents in permissive mode [default=false].
    bool collapse_spaces;
  };
  
  /** [impl] HTML Creator.
   */
  struct HtmlCreator : public XmlCreator {
    HtmlCreator();
  };
  
    
  struct ImageAttachment : public DocumentAttachment {
    const String url, type;
    Image* ima;
    
    ImageAttachment(const String& _url, const String& _type, Image* _ima)
    : url(_url), type(_type), ima(_ima) {}
    
    virtual const String& getUrl() const {return url;}
    virtual const String& getType() const {return type;}
    virtual bool isLoaded() const;
    virtual int  load(Document*);
  };
  
    
  struct CssAttachment : public DocumentAttachment {
    const String url, type;
    XmlDocument* doc;
    int stat;
    
    CssAttachment(const String& _url, const String& _type, XmlDocument* _doc)
    : url(_url), type(_type), doc(_doc), stat(0) {}
    
    virtual const String& getUrl() const {return url;}
    virtual const String& getType() const {return type;}
    virtual bool isLoaded() const {return stat > 0;}
    virtual int  load(Document*);
  };
  
}
#endif // UBIT_CORE_DOM_H_

