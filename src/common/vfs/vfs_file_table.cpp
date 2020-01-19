/***************************************************************************
  vfs_file_table.cpp  - Main VFS file table.
  --------------------------------------------------------------------------
  begin                : 2005-03-07
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  copyright            : (C) 2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#include "common/vfs/vfs_file_table.h"
#include "common/console/console.h"
#include "common/exception.h"

//#define STILL_HERE cdebug << "STILL HERE ON LINE " << __LINE__ << std::endl

namespace VFS
{

void File_instance::finalize() throw()
{
	if(!header)
		cassert << "Trying to finalize file instance with no header."
			<< std::endl;

	//Firstly, if we provode the filename, and it is not from heap,
	//switch filename to next instance. Skip this if there is no next
	//instance, as header will go down.
	if(header->filename.get_string() == filename && 
			!header->filename_in_heap) {

		//The else if is a sanity check.
		if(next) {
			header->filename.alter(next->filename);
		} else if(prev) {
			cassert << "BAD: File name provoded by "
				<< "lesser-priority instance of file."
				<< std::endl;
		}
	}
	
	//Unlink from chain from instances.
	if(prev)
		prev->next = next;
	if(next)
		next->prev = prev;
	prev.nullify();
	next.nullify();

	//Release the header. This frees it if needed.
	header.nullify();

	//Unlink from chain of instances from source. This propagates
	//the destruction of source.
	if(next_source)
		next_source->prev_source = prev_source;
	if(prev_source)
		prev_source->next_source = next_source;
	prev_source.nullify();
	next_source.nullify();
}

File::File(VFS::Files_dictionary_t dictionary, const char* _filename,
		bool from_heap) throw(std::bad_alloc)
	: filename(_filename)
{
	this->dictionary = dictionary;
	this->filename_in_heap = from_heap;
}

void File::finalize() throw()
{
	//Release it from the dictionary.
	dictionary->erase(filename);

	//Free the name, if apporiate.
	if(filename_in_heap)
		delete [] (filename.get_string());
}

pointer::Weak<VFS::File> lookup_header(VFS::Files_dictionary_t dictionary,
	const char* filename) throw()
{
	Inplace_alterable _filename(filename, true);

	//Found in dictionary?
	if(dictionary->count(_filename) == 0)
		return pointer::Weak<VFS::File>();

	return (*dictionary)[_filename];
}

pointer::Weak<VFS::File_instance> lookup_instance(
	VFS::Files_dictionary_t dictionary, const char* filename) throw()
{
	pointer::Weak<VFS::File> header;

	header = lookup_header(dictionary, filename);
	if(!header)
		return pointer::Weak<VFS::File_instance>();

	return header->instances;
}

pointer::Strong<VFS::File_instance> add_file_instance(
	VFS::Files_dictionary_t dictionary, const char* filename, 
	pointer::Strong<VFS::Memory_arena> mapping, 
	pointer::Weak<VFS::File_instance> source_last, bool filename_mapped) 
	throw(std::bad_alloc)
{
	
	pointer::Strong<VFS::File_instance> new_instance;
	
	bool freeable = false;

	//Look up for header of file.
	pointer::Strong<VFS::File> header = 
		lookup_header(dictionary, filename);

	//If not mapped, and no header with heapified one, create persistent
	//name in heap.
	if(!filename_mapped && (!header || !header->filename_in_heap)) {
		char* tmp = new char[strlen(filename) + 1];
		strcpy(tmp, filename);
		filename = tmp;
		freeable = true;
	}
	
	//If no header, create one.
	if(!header) {
		try {
			header = pointer::Strong<VFS::File>(
				new VFS::File(dictionary, filename, 
				!filename_mapped));
		} catch(...) {
			//Free filename if necressary.
			if(freeable) 
				delete [] filename;
			throw;
		}
	}

	//Change filename to heap one if apporiate.
	if(!filename_mapped && !header->filename_in_heap) {
		header->filename.alter(filename);
		header->filename_in_heap = true;
	}

	//Add instance. Filename should be properly destroyed if this fails.
	new_instance = pointer::Strong<VFS::File_instance>(new
		VFS::File_instance());

	new_instance->mapped_at = mapping;
	new_instance->filename = filename;
	new_instance->next = header->instances;
	new_instance->prev_source = source_last;
	new_instance->header = header;
	
	header->instances = new_instance;

	//Link it to list of source.
	if(source_last)
		source_last->next_source = new_instance;

	//Finally, add it to dictionary if apporiate.
	if(!dictionary->count(header->filename))
		dictionary->insert(std::make_pair(header->filename, header));
	
	return new_instance;
}


}
