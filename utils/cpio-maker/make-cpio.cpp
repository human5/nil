/***************************************************************************
  make-cpio.cpp  - Make CPIOs out of directories.
  --------------------------------------------------------------------------
  begin                : 2004-11-20
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

  last change          : 2005-01-06
  by                   : H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi
  changedescription    : Make CPIO-maker work after file reorg. 

  copyright            : (C) 2004 by H. Ilari Liusvaara
  email                : hliusvaa@cc.hut.fi

 ***************************************************************************/

#include "common/vfs/loader.h"
#include "common/console/console.h"

//Fucking public members in loader.
Animations::Animations() {};
Animations::~Animations() {};
void Animations::init(Loader* l) {};
Sounds::Sounds() {};
Sounds::~Sounds() {};
void Sounds::init(Loader* l) {};
Graphics::Graphics() {};
Graphics::~Graphics() {};
void Graphics::init(Loader* l) {};
Fonts::Fonts() {};
Fonts::~Fonts() {};
void Fonts::init(Loader* l) {};

class console_stdout : public Console
{
	void _put_output(Console::Severity s, const char* src, const char* out)
		throw()
	{
		fprintf(stderr, "%s", out);
		fflush(stderr);
	}
};

//Let's test CPIO loader's ability to cope with seriously faulty input...
char _binary_builtin_cpio_start[6] = {0, 0, 0, 0, 0, 0};
char _binary_builtin_cpio_size;

int main(int argc, char** argv)
{
	console_stdout c;

	Loader loader;
	bool have_success = false;

	//Supress messages to console from loader construction.
	master_console.set_chattiness(Console::ERROR);
	master_console.add_slave(c);

	if(argc < 3)
	{
		cerror << "Syntax: " << argv[0] << 
			" lump-to-make source1 [source2...]\n";
		cerror << "Sources can be directories or existing CPIO "
			"lumps.\n";
		exit(1);
	}

	for(int i = 2; i < argc; i++)
		if(loader.addsource(argv[i]) >= 0)
			have_success = true;

	if(!have_success)
	{
		cerror << "No data loaded, will not make lump.\n";
		exit(2);
	}

	int lumpsize = loader.save_lump(NULL, 0);
	char* buffer = (char*)malloc(lumpsize);
	if(buffer == NULL)
	{
		cerror << "Not enough memory to allocate output buffer.\n";
		exit(3);
	}

	loader.save_lump(buffer, lumpsize);

	FILE* out = fopen(argv[1], "wb");
	if(out == NULL)
	{
		cerror << "Can't open output file \"" << argv[1] << "\".\n";
		exit(4);
	}

	if(fwrite(buffer, 1, lumpsize, out) != lumpsize)
	{
		cerror << "Can't write output file \"" << argv[1] << "\".\n";
		exit(4);
	}

	fclose(out);
	free(buffer);
}
