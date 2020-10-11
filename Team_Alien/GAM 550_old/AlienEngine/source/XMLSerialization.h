////////////////////////////////////////////////////////////////
//
//	XMLSerialization.h
//	implements the Serialization interface for the XML serialization. Uses external library "rapidxml.hpp" for parsing xml
//
//	Author: Santosh Shedbalkar
//	Copyright 2011, Digipen Institute of Technology
//
////////////////////////////////////////////////////////////////
#ifndef XML_SERIALIZATION_H
#define XML_SERIALIZATION_H
//#pragma message("Compiling: "__FILE__)
//
#include "rapidxml.hpp"
#include "Serialization.h"
//#include "Utility.h"
//

namespace Framework
{
	///Serializer for a simple text file format.
	class XMLSerializer: public ISerializer
	{
//#pragma message("Including Class: XMLSerializer")
	public:
		friend class XMLReader;
		XMLSerializer();
		virtual ~XMLSerializer();
		XMLSerializer(const XMLSerializer& orig);
		bool readProperty(const char* prop, int& i) const;
		bool readProperty(const char* prop, float& f) const;
		bool readProperty(const char* prop, std::string& str) const;
		bool readProperty(const char* prop, ISerializer& stream);
		bool readProperty(const char* prop, std::vector<ISerializer*>& vec) const;
		bool readProperty(const char* prop, float* vec, int count ) const;
		//
		bool readValue(const char* prop, int& i) const;
		bool readValue(const char* prop, float& f) const;
		bool readValue(const char* prop, std::string& str) const;
		bool readValue( const char* prop, float* vec, int count ) const;
		//
		bool writeProperty( const char* key, const char* value );
		bool writeNodeAndValue( const char* node, const char* value );
		bool writeNode( const char* node );
		//
		void allocateNew();
		//
		bool goToParent();
		//
		ISerializer* clone();
		ISerializer* clone() const;
		bool isGood() const;
		char* name() const;
		//
		static bool saveNodeToStream( ISerializer& stream, std::ostream *os );
	protected:
		rapidxml::xml_node<>* data();
		void data(rapidxml::xml_node<>* xml);
	private:
		XMLSerializer(rapidxml::xml_node<>* xml, rapidxml::xml_document<> *doc);
		rapidxml::xml_node<>* _xml;
		rapidxml::xml_document<> *_doc;
		bool _owner;
	};
	//
	class XMLReader
	{
	public:
		XMLReader();
		virtual ~XMLReader();
		bool open(const char* file);
		XMLSerializer data();
		//
	private:
		rapidxml::xml_document<> _doc;
		char* _buffer;
	};
}
//
#endif