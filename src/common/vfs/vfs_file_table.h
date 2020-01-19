/***************************************************************************
  vfs_file_table.h  - Main VFS file table.
  --------------------------------------------------------------------------
  begin                : 2005-03-07
  by                   : H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

  copyright            : (C) 2005 by H. Ilari Liusvaara (hliusvaa@cc.hut.fi)

 ***************************************************************************/

#ifndef _VFS_FILE_TABLE_H_
#define _VFS_FILE_TABLE_H_

#include "common/vfs/vfs_memory_arena.h"
#include "common/smartpointer/pointer.h"
#include "common/vfs/vfs_inplace_alterable.h"
#include <map>

namespace VFS
{
struct File;
struct File_instance;

/**
 * \brief Dictionary type.
 *
 * This is the type of global file dictionary.
 *
 * \note Maps don't have finalize(), so this can't be smart pointer.
 *
 */
typedef std::map<VFS::Inplace_alterable, pointer::Weak<VFS::File> >*
	Files_dictionary_t;

/**
 * \brief An instance of file.
 *
 * This structure groups information about an instance of a file.
 *
 */
struct File_instance
{
/**
 * \brief Pointer to mapping of file.
 *
 * This points to memory arena object provoding file contents.
 *
 */
	pointer::Strong<VFS::Memory_arena> mapped_at;

/**
 * \brief Name of file.
 * 
 * Pointer to file name array.
 *
 */
	const char* filename;

/**
 * \brief Next instance of same name.
 *
 * Less priority version of same name.
 *
 */
	pointer::Weak<VFS::File_instance> next;

/**
 * \brief Previous instance of same name.
 *
 * More priority version of same name.
 *
 */
	pointer::Weak<VFS::File_instance> prev;

/**
 * \brief Next instance from same data source.
 *
 * Next file instance from same data source.
 *
 */
	pointer::Strong<VFS::File_instance> next_source;

/**
 * \brief Previous instance from same data source.
 *
 * Previous file instance from same data source.
 *
 */
	pointer::Weak<VFS::File_instance> prev_source;

/**
 * \brief Header structure for file.
 *
 * The header structure for file.
 *
 */
	pointer::Strong<VFS::File> header;

/**
 * \brief Finalizer.
 *
 * Finalizer needed for using smart pointers. Disconnects all ties to 
 * other Files and File_instances.
 *
 */
	void finalize() throw();
};

/**
 * \brief File
 *
 * This structure combines information about file.
 *
 */
struct File
{
/**
 * \brief Constructor.
 *
 * Construct file header and set name of file.
 *
 * \param dictionary Dictionary to use.
 * \param filename Name of the file. Must be persistent.
 * \param from_heap Filename is from heap?
 *
 * \exception std::bad_alloc Not enough memory.
 * 
 */
	File(VFS::Files_dictionary_t dictionary, const char* filename,
		bool from_heap) throw(std::bad_alloc);

/**
 * \brief The name of file.
 *
 * This contains the name of the file. It must either come from heap or from
 * primary instance of the file.
 *
 */
	VFS::Inplace_alterable filename;

/**
 * \brief Does the file name come from heap?
 *
 * This boolean is TRUE if file name is in heap, FALSE if it comes from primary
 * instance.
 *
 */
	bool filename_in_heap;

/**
 * \brief Chain of instances.
 *
 * This points to primary instance of the file.
 *
 */
	pointer::Weak<VFS::File_instance> instances;

/**
 * \brief The dictionary.
 *
 * This points to dictionary this file is from.
 *
 */
	Files_dictionary_t dictionary;

/**
 * \brief Finalizer.
 *
 * Finalizer needed for using smart pointers. Frees filename if apporiate.
 *
 */
	void finalize() throw();

};

/**
 * \brief Look up header of file.
 *
 * This looks up header for given filename in dictionary.
 *
 * \param dictionary The dictionary to use.
 * \param filename The name of the file.
 *
 * \return The file header, or pointer to nowhere if not found.
 *
 */
pointer::Weak<VFS::File> lookup_header(VFS::Files_dictionary_t dictionary,
	const char* filename) throw();

/**
 * \brief Look up primary instance of file.
 *
 * This looks up primary instance for given filename in dictionary.
 *
 * \param dictionary The dictionary to use.
 * \param filename The name of the file.
 *
 * \return The file instance, or pointer to nowhere if not found.
 *
 */
pointer::Weak<VFS::File_instance> lookup_instance(
	VFS::Files_dictionary_t dictionary, const char* filename) throw();

/**
 * \brief Add file instance.
 *
 * This function adds instance for given file to given dictionary.
 *
 * \param dictionary The dictionary to use.
 * \param filename The name of the file.
 * \param mapping Contents mapping to use.
 * \param source_last Last file instance in source chain.
 * \param filename_mapped If TRUE, filename is mapped.
 *
 * \return pointer to the new instance.
 *
 * \exception std::bad_alloc Not enough memory to add the instance.
 *
 */
pointer::Strong<VFS::File_instance> add_file_instance(
	VFS::Files_dictionary_t dictionary, const char* filename, 
	pointer::Strong<VFS::Memory_arena> mapping, 
	pointer::Weak<VFS::File_instance> source_last, bool filename_mapped) 
	throw(std::bad_alloc);

}
#endif
