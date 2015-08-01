#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include "fileinterface.h"
#include "basefile.h"
#include "resourcefile.h"
#include "ramfile.h"

#include <list>

struct FileInfo 
{
	int nFilesize;
};

struct FileChunk
{
	uint8*		m_pData;
	FileInfo	m_FileInfo;
};

class CResourceManager
{
public:
	CResourceManager()
	{
	}

	~CResourceManager()
	{	
		m_DirectoryList.clear();

		ResourceFileList::iterator iter = m_ResourceFileList.begin();
		while(iter != m_ResourceFileList.end())
		{
			delete (*iter);
			++iter;
		}
		m_ResourceFileList.clear();

		FileCache::iterator iterCache = m_FileCache.begin();
		while(iterCache != m_FileCache.end())
		{
			delete [] iterCache->second.m_pData;
			++iterCache;
		}
		m_FileCache.clear();

	}

	GEN_RESULT AddResourceFile(const char* szFilename)
	{
		FileStats stats;
		bool bFound = m_FileInterface.StatFile(szFilename, &stats);
		if(bFound)
		{
			ResourceFile* pResourceFile = new ResourceFile;
			pResourceFile->Open(szFilename);
			m_ResourceFileList.push_back(pResourceFile);

			return RESULT_OK;
		}

		return RESULT_ERROR;
	}

	GEN_RESULT AddDirectory(const char* szFilename)
	{
		//m_DirectoryList
		if(szFilename[0] != '\0')
		{
			m_DirectoryList.push_back(szFilename);
			return RESULT_OK;
		}

        return RESULT_ERROR;		
	}

	BaseFile* CacheFile(const char* szFilename, BaseFile* pFile, int nSize)
	{
		FileChunk chunk;
		chunk.m_FileInfo.nFilesize = nSize;

		chunk.m_pData = new uint8[nSize];
		if(pFile->Read(chunk.m_pData, nSize) == RESULT_OK)
		{
			pFile->Close();

			m_FileCache[szFilename] = chunk;

			RamFile* pRamFile = new RamFile;
			pRamFile->Open(chunk.m_pData, nSize);
			return (BaseFile*)pRamFile;
		}

		return NULL;
	}

	BaseFile* FindFile(const char* szFilename, FileInfo* pFileInfo)
	{
		//look in the file cache first
		FileCache::iterator cachedFile = m_FileCache.find(szFilename);
		if(cachedFile != m_FileCache.end())
		{
			// we've got it.. return a BaseFile pointer..
			pFileInfo->nFilesize = cachedFile->second.m_FileInfo.nFilesize;
			
			RamFile* pRamFile = new RamFile;
			if(pRamFile)
			{
				pRamFile->Open(cachedFile->second.m_pData, pFileInfo->nFilesize);
				return (BaseFile*)pRamFile;
			}			
		}

		// The file was not cached, check the local disk, then resource files.

		//look on disk before looking at the resource file
		//look trough our directories
		std::string sFullPath;
		DirectoryList::iterator dirIter = m_DirectoryList.begin();
		while(dirIter != m_DirectoryList.end())
		{
			sFullPath = *dirIter;
			sFullPath += "\\";
			sFullPath += szFilename;

			BaseFile* pFile = m_FileInterface.OpenFile(sFullPath.c_str(), BINARY_FILE, READONLY);
			if(pFile)
			{
				FileStats stats;
				m_FileInterface.StatFile(sFullPath.c_str(), &stats);
				pFileInfo->nFilesize = stats.nSize;

				return CacheFile(sFullPath.c_str(), pFile, stats.nSize);
			}

			++dirIter;
		}



		//traverse the resource files
		ResourceFileList::iterator iter = m_ResourceFileList.begin();
		while(iter != m_ResourceFileList.end())
		{
			ResourceFileStats stats;
			BaseFile* pFile = (*iter)->Find(szFilename, &stats);

			if(pFile)
			{
				pFileInfo->nFilesize = stats.nFileSize;
	
				return CacheFile(szFilename, pFile, stats.nFileSize);
			}

			++iter;
		}

		return NULL;
	}

	static CResourceManager& GetSingleton()
	{
		static CResourceManager g_ResourceMgr;
		return g_ResourceMgr;
	}

protected:

	typedef std::list<ResourceFile*> ResourceFileList;
	typedef std::map<std::string, FileChunk> FileCache;
	typedef std::vector<std::string> DirectoryList;

	ResourceFileList m_ResourceFileList;	// Resource files
	FileInterface m_FileInterface;			// DOS Tree
	FileCache m_FileCache;					// File cache
	DirectoryList m_DirectoryList;
};

#endif