/*
 *  doc.h: Document Element
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

#ifndef UBIT_CORE_DOC_H_
#define UBIT_CORE_DOC_H_

#include <list>
#include <map>
#include <memory>

#include <ubit/udefs.hpp>
#include <ubit/core/string.h>
#include <ubit/ui/boxes.h>

namespace ubit {
  
  class DocumentCreator;
  class DocumentAttachment;
  
  /** DocumentAttachment List.
   */
  typedef std::list<DocumentAttachment*> DocumentAttachments;
  
    /** Document Factory: use this class to read documents.
   * See class Document for info on "documents".
   */
  class DocumentFactory {
  public:
    DocumentFactory();
    virtual ~DocumentFactory();
    
    static DocumentFactory& getDefaultFactory();
    /**< returns the default factory for reading/creating documents.
     */
    
    virtual Document* create(const String& name);
    /**< creates an empty document.
     * creates an instance of the appropriate Document subclass depending
     * on 'name' suffix.
     */
    
    virtual Document* load(const String& name, const String& buffer);
    /**< loads a document from a buffer in memory.
     * creates an instance of the appropriate Document subclass depending
     * on 'name' suffix.
     */
    
    virtual Document* read(const String& filename);
    /**< reads a document from a file.
     * reads and creates an instance of the appropriate Document subclass
     *  depending on 'filename' suffix.
     */
    
    virtual int getStatus() const;
    /**< status of the last (read/load/create) operation.
     * value is:
     * - > 0 if the document could be created
     * - < 0 if the file cant be read (see UFilestat)
     * - = 0 if the type of the document is unknown (see getCreator, addCreator)
     */ 
    
    virtual void saveErrors(bool);
    virtual String* getErrors() const; 
    /**< return last errors messages if saveErrors() was set to true.
     * errors are printed on stderr otherwise.
     */
    
    virtual DocumentCreator* getCreator(const String& type);
    /**< returns the creator for this type (if any, null otherwise).
     * by convention, the 'type' is the suffix of the file
     * (e.g. "html", "txt", "cpp" ...)
     *
     * Note: certains creators (such as the HTML or XML creators)
     * have specific options. This function provides a mean to retrieve
     * the actual creator and to change its options.
     */
    
    virtual DocumentCreator* getDefaultCreator();
    ///< returns the default document creator.
    
    virtual void addCreator(const String& type, DocumentCreator&);
    ///< adds a document creator.
    
    virtual void setDefaultCreator(DocumentCreator&);
    ///< changes the creator that is used when the document type is unknown.
    
  protected:
    struct Comp {
      bool operator()(const String* s1, const String* s2) const;
    };
    typedef std::map<const String*, DocumentCreator*, Comp> Map;
    Map rmap;
    DocumentCreator* default_creator;
    int stat;
    std::unique_ptr<String> errors;
  };
  

    /** Generic Document
   *
   * a Document is just a tree (or acyclic graph) of various Ubit objects.
   * It can be modified in the same way as other Ubit instance trees.
   *
   * Document can't be instantiated directly, only appropriate subclasses
   * can be (such as XmlDocument, etc.) 
   * 
   * DocumentFactory::read() reads a document file and creates an instance 
   * of the appropriate Document subclass depending on the file suffix
   */
  class Document : public Box {
  public:
    UCLASS(Document)
    
    static DocumentFactory& getDefaultFactory();
    ///< returns the default factory (for reading/creating documents).
    
    virtual ~Document();
    
    
    virtual bool isEditable() const;
    virtual void setEditable(bool state = true);
    
    virtual const String& getPath() const {return *ppath;}
    ///< returns the pathname of the document.
    
    virtual void setPath(const String& path);
    virtual void makePath(String& fullpath, const String& url);
    /**< creates a full pathname from an attachment or an URL.
     * returns true if the fullname is a local filename.
     * returns false otherwise (typically is the fullname starts with http:)
     */
    
    virtual const DocumentAttachments* getAttachments() const {return null;}
    ///< returns the objects (images, stylesheets, etc) that are attached to this document.
    
    virtual int loadAttachments(bool reload = false) {return 0;}
    /**< loads the attachments.
     * - if 'reload' is false: load the attachments that are not already loaded
     * - if 'reload' is true: reload all the attachments
     * - returns: the number of attachments that are still unloaded
     */
    
    virtual bool loadAttachment(DocumentAttachment*, bool reload = false) {return false;}
    ///< loads this attachment.
    
    virtual void addLinkCallback(const Child& cond_slash_callback);
    /**< adds a callback that observes events in the document's hyperlinks.
     * 'cond_slash_callback' is condition/callback expression such as: 
     * <pre>   UOn::action / ucall(obj, &Obj::foo)   // (obj type is Obj*)  
     * </pre>
     * The callback (ie. 'foo') will be fired when the condition (ie. 'UOn::action')
     * occurs in a link (typically a href anchor) of this document.
     */
    
    
    virtual void linkEventCB(InputEvent&, const String* path);
    
  protected:
    Document();
    Document(const String& path);
    ///< NB: the constr is protected: use read() or a subclass to create a Document.
    
  private:
    std::unique_ptr<String> ppath;
    std::unique_ptr<Element> call_list;
    Document(const Document&);             // these operators are forbidden
    Document& operator=(const Document&);
  };
  
    /** Document attachment (image, stylesheet, etc.)
   */
  class DocumentAttachment {
  public:
    virtual ~DocumentAttachment() {}
    virtual const String& getUrl() const = 0;
    virtual const String& getType() const = 0;
    virtual bool isLoaded() const = 0;
    virtual int  load(Document*) = 0;
  };
  
    /** Document source (file, buffer, etc.).
   */
  struct DocumentSource {
    enum From {PATHNAME, BUFFER, SCRATCH};
    
    DocumentSource(int from, const String* path, String* errors, const String* buffer = null);
    ///< note that path and errors are not duplicated.
  
    int from;
    int stat;
    std::unique_ptr<const String> path, fullpath;
    const String* buffer;
    String* errors;
  };
  
  
    /** Document creator interface (intended to be subclassed).
   */
  class DocumentCreator {
  public:
    virtual ~DocumentCreator() {}
    virtual Document* create(DocumentSource&) = 0;
    ///< create or load the document specified by the DocumentSource.
  };
  
  
    /**< Base of flat documents that just contains flat text or an image
   * see also: XmlDocument for XML "hierarchical" documents.
   */
  class FlatDocument : public Document {
  public:
    FlatDocument(const String& _pathname);
    virtual bool isEditable() const;
    virtual void setEditable(bool state = true);
    std::unique_ptr<class TextEdit> edit;
  };
  
  
    /**< Documents that needs a plugin to be displayed. 
   */
  class PluginDocument : public Document {
  public:
    PluginDocument(const String& _pathname);
    //virtual void setEditable(bool state = true) {}
  };
  
}
#endif // UBIT_CORE_DOC_H_
