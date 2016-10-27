#pragma once
#include "GCoreStd.h"

enum SET_DATA_TYPES
{
	// Gameplay semantics
	NULL_DATA = 0,
	BOOL_DATA,
	INT_DATA,
	FLOAT_DATA,
	STRING_DATA,
	VERTEX_DATA,
	VECTOR_DATA,
	INDEX_DATA,
	FACE_DATA,
	MATRIX_DATA,
};

class GParser
{
public:
	FILE*	m_pStream;
	TCHAR	m_pBuffer[256];
	TCHAR	m_pString[256];
public:
	BOOL	ComparisonData(TCHAR* pDestString, TCHAR* pSrcString = NULL);
	bool	ErrorCloseStream(TCHAR* pMsg);
	BOOL	OpenStream(const TCHAR* pFileName);
	BOOL    CloseStream() { fclose(m_pStream); return TRUE; };
	BOOL	GetData(VOID* pData, INT DataType);
	// 전체파일에서 해당 문자열을 찾을때 까지 검색하고 파일포인터를 다음라인으로 옮긴다.
	BOOL	GetDataFromFileLoop(TCHAR* pString = NULL, VOID* pData = NULL, INT DataType = NULL_DATA);
	// 다음라인을 대상으로 문장열을 찾고 다음라인으로 파일포인터를 옮긴다.
	BOOL	GetDataFromFileNext(TCHAR* pString = NULL, VOID* pData = NULL, INT DataType = NULL_DATA);
	// 원하는 문자열을 찾고 찾은 문자열 시작위치로 옮긴다
	BOOL	GetDataFromFileString(TCHAR* pString = NULL, VOID* pData = NULL, INT DataType = NULL_DATA);
	// 다음라인을 대상으로 해당문자열을 찾고 검색을 실폐하면 이전 라인으로 파일포인터를 옮긴다.
	BOOL	GetDataFromFile(TCHAR* pString = NULL, VOID* pData = NULL, INT DataType = NULL_DATA);
	BOOL    GetNodeName(TCHAR* pString = NULL, VOID* pData = NULL, INT DataType = NULL_DATA);
public:
	GParser(void);
	virtual ~GParser(void);
};
