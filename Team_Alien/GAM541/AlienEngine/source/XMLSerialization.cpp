#include <fstream>
#include "XMLSerialization.h"
#include "Utility.h"
#include "DebugTools.h"
//
namespace Framework
{
	bool XMLReader::open(const char* file)
	{
		int length;

		std::ifstream is;
		is.open (file, std::ios::binary );

		if(is.is_open())
		{
			// get length of file:
			is.seekg(0, std::ios::end);
			length = static_cast<int>(is.tellg());
			is.seekg(0, std::ios::beg);
			//
			// allocate memory:
			delete[] _buffer;
			_buffer = NULL;
			_buffer = new char [length+1];
			//
			// read data as a block:
			is.read (_buffer, length);
			is.close();
			_buffer[length] = '\0';
			_doc.parse<rapidxml::parse_declaration_node | rapidxml::parse_no_data_nodes>(_buffer);
			//
			return true;
		}
		else
		{
			std::cout<<"Cant open file: " <<file<<"\n";
			return false;
		}
	}
	XMLSerializer XMLReader::data()
	{
		return XMLSerializer(_doc.first_node("root"));
		//return XMLSerializer(_doc.first_node());
	}
	XMLReader::XMLReader()
		:_doc(),
		_buffer(NULL)
	{
		//
	}
	XMLReader::~XMLReader()
	{
		delete[] _buffer;
	}
	/////////////////////////////////////////////////////
	XMLSerializer::XMLSerializer()
		:_xml(NULL)
	{
		//
	}
	XMLSerializer::XMLSerializer(const XMLSerializer& orig):
		_xml(orig._xml)
	{
		//
	}
	XMLSerializer::XMLSerializer(rapidxml::xml_node<>* xml)
		:_xml(xml)
	{
		//
	}
	XMLSerializer::~XMLSerializer()
	{
		//
	}
	rapidxml::xml_node<>* XMLSerializer::data()
	{
		return _xml;
	}
	void XMLSerializer::data(rapidxml::xml_node<>* xml)
	{
		_xml = xml;
	}
	bool XMLSerializer::readProperty(const char* prop, int& i) const
	{
		rapidxml::xml_attribute<>* attr = _xml->first_attribute(prop);
		if(attr)
		{
			from_string<int>(i, attr->value(), std::dec);
			return true;
		}
		return false;
	}
	bool XMLSerializer::readValue(const char* prop, int& i) const
	{
		rapidxml::xml_node<> *node = _xml->first_node(prop);
		if(node)
		{
			from_string<int>(i, node->value(), std::dec);
			return true;
		}
		return false;
	}
	bool XMLSerializer::readProperty(const char* prop, float& f) const
	{
		rapidxml::xml_attribute<>* attr = _xml->first_attribute(prop);
		if(attr)
		{
			from_string<float>(f, attr->value(), std::dec);
			return true;
		}
		return false;
	}
	bool XMLSerializer::readValue(const char* prop, float& f) const
	{
		rapidxml::xml_node<> *node = _xml->first_node(prop);
		if(node)
		{
			from_string<float>(f, node->value(), std::dec);
			return true;
		}
		return false;
	}
	bool XMLSerializer::readProperty(const char* prop, std::string& str) const
	{
		rapidxml::xml_attribute<>* attr = _xml->first_attribute(prop);
		if(attr)
		{
			str = attr->value();
			return true;
		}
		return false;
	}
	bool XMLSerializer::readValue(const char* prop, std::string& str) const
	{
		rapidxml::xml_node<> *node = _xml->first_node(prop);
		if(node)
		{
			str = node->value();
			return true;
		}
		return false;
	}
	bool XMLSerializer::readProperty(const char* prop, ISerializer& stream)
	{
		XMLSerializer* str = dynamic_cast<XMLSerializer*>(&stream);
		if(str)
		{
			str->data(_xml->first_node(prop));
			return str->data()!=0;
		}
		return false;
	}
	bool XMLSerializer::readProperty(const char* prop, std::vector<ISerializer*>& vec) const
	{
		rapidxml::xml_node<> *node;
		node = _xml->first_node(prop);
		while(node)
		{
			vec.push_back(new XMLSerializer(node));
			node = node->next_sibling(prop);
		}
		return vec.size()>0;
	}
	bool XMLSerializer::readProperty(const char* prop, Vec2& vec) const
	{
		std::string str;
		std::vector<float> v;
		readProperty(prop, str);
		if(!str.empty())
		{
			readArrayFromString(str, v);
			vec.x = v[0];
			vec.y = v[1];
			return true;
		}
		return false;
	}
	bool XMLSerializer::readValue(const char* prop, Vec2& vec) const
	{
		std::string str;
		std::vector<float> v;
		readValue(prop, str);
		if(!str.empty())
		{
			readArrayFromString(str, v);
			vec.x = v[0];
			vec.y = v[1];
			return true;
		}
		return false;
	}
	bool XMLSerializer::readProperty(const char* prop, Vec3& vec) const
	{
		std::string str;
		std::vector<float> v;
		readProperty(prop, str);
		if(!str.empty())
		{
			readArrayFromString(str, v);
			vec.x = v[0];
			vec.y = v[1];
			vec.z = v[2];
			return true;
		}
		return false;
	}
	bool XMLSerializer::readValue(const char* prop, Vec3& vec) const
	{
		std::string str;
		std::vector<float> v;
		readValue(prop, str);
		if(!str.empty())
		{
			readArrayFromString(str, v);
			vec.x = v[0];
			vec.y = v[1];
			vec.z = v[2];
			return true;
		}
		return false;
	}
	bool XMLSerializer::readProperty(const char* prop, Vec4& vec) const
	{
		std::string str;
		std::vector<float> v;
		readProperty(prop, str);
		if(!str.empty())
		{
			readArrayFromString(str, v);
			vec.x = v[0];
			vec.y = v[1];
			vec.z = v[2];
			vec.w = v[3];
			return true;
		}
		return false;
	}
	bool XMLSerializer::readValue(const char* prop, Vec4& vec) const
	{
		std::string str;
		std::vector<float> v;
		readValue(prop, str);
		if(!str.empty())
		{
			readArrayFromString(str, v);
			vec.x = v[0];
			vec.y = v[1];
			vec.z = v[2];
			vec.w = v[3];
			return true;
		}
		return false;
	}
	char* XMLSerializer::name() const
	{
		return _xml->name();
	}
	bool XMLSerializer::isGood() const
	{
		return _xml!=0;
	}
	ISerializer* XMLSerializer::clone()
	{
		return new XMLSerializer(_xml);
	}
	ISerializer* XMLSerializer::clone() const
	{
		return new XMLSerializer(_xml);
	}
	bool XMLSerializer::goToParent()
	{
		_xml = _xml->parent();
		return _xml != 0;
	}
}
//