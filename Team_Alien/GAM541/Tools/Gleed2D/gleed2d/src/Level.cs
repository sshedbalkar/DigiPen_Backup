﻿using System;
using System.Collections.Generic;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Content;
using System.ComponentModel;
using System.IO;
using System.Xml;
using System.Xml.Serialization;


namespace GLEED2D
{
    public partial class root
    {
        /// <summary>
        /// The name of the root.
        /// </summary>
        [XmlAttribute()]
        public String Name;

        [XmlIgnore()]
        [XmlAttribute()]
        public bool Visible;

        /// <summary>
        /// A root contains several Layers. Each Layer contains several Items.
        /// </summary>
        public Level Level;

        /// <summary>
        /// A Dictionary containing any user-defined Properties.
        /// </summary>
        /// 
        [XmlIgnore()]
        public SerializableDictionary CustomProperties;


        public root()
        {
            Visible = true;
            Level = new Level("Level");
            CustomProperties = new SerializableDictionary();
        }

        public static root FromFile(string filename, ContentManager cm)
        {
            FileStream stream = File.Open(filename, FileMode.Open);
            XmlSerializer serializer = new XmlSerializer(typeof(root));
            root root = (root)serializer.Deserialize(stream);
            stream.Close();


            foreach (Layer layer in root.Level.Layers)
            {
                foreach (Item item in layer.Items)
                {
                    item.CustomProperties.RestoreItemAssociations(root);
                    item.load(cm);
                }
            }
            

            return root;
        }

        public static root FromFile(string filename)
        {
            FileStream stream = File.Open(filename, FileMode.Open);
            XmlSerializer serializer = new XmlSerializer(typeof(root));
            root root = (root)serializer.Deserialize(stream);
            stream.Close();

           

            return root;
        }


        public Item getItemByName(string name)
        {
            foreach (Layer layer in Level.Layers)
            {
                foreach (Item item in layer.Items)
                {
                    if (item.Name == name) return item;
                }
            }
            return null;
        }

        public Layer getLayerByName(string name)
        {
            foreach (Layer layer in this.Level.Layers)
            {
                if (layer.Name == name) return layer;
            }
            return null;
        }

        public void draw(SpriteBatch sb)
        {
            foreach (Layer layer in this.Level.Layers) layer.draw(sb);
        }


    }

    public partial class Level
    {
        /// <summary>
        /// The name of the level.
        /// </summary>
        [XmlAttribute()]
        public String Name;

        [XmlIgnore()]
        [XmlAttribute()]
        public bool Visible;

        /// <summary>
        /// A root contains several Layers. Each Layer contains several Items.
        /// </summary>
        public List<Layer> Layers;

        /// <summary>
        /// A Dictionary containing any user-defined Properties.
        /// </summary>
        /// 
        [XmlIgnore()]
        public SerializableDictionary CustomProperties;


        public Level()
        {
            Visible = true;
            Layers = new List<Layer>();
            CustomProperties = new SerializableDictionary();
        }


    }

     
    public partial class Layer
    {
        /// <summary>
        /// The name of the layer.
        /// </summary>
        [XmlAttribute()]
        public String Name;

        /// <summary>
        /// Should this layer be visible?
        /// </summary>
        /// 
        //[XmlIgnore()]
        [XmlAttribute()]
        public bool Visible;

        /// <summary>
        /// The list of the items in this layer.
        /// </summary>
        public List<Item> Items;

        /// <summary>
        /// The Scroll Speed relative to the main camera. The X and Y components are 
        /// interpreted as factors, so (1;1) means the same scrolling speed as the main camera.
        /// Enables parallax scrolling.
        /// </summary>
        /// 
        [XmlIgnore()]
        public Vector2 ScrollSpeed;

        /// <summary>
        /// A Dictionary containing any user-defined Properties.
        /// </summary>
        /// 
        [XmlIgnore()]
        public SerializableDictionary CustomProperties;


        public Layer()
        {
            Items = new List<Item>();
            ScrollSpeed = Vector2.One;
            CustomProperties = new SerializableDictionary();
        }

        public void draw(SpriteBatch sb)
        {
            if (!Visible) return;
            foreach (Item item in Items) item.draw(sb);
        }

    }

        //DEFAULT ITEMS
    [XmlInclude(typeof(TextureItem))]
    [XmlInclude(typeof(RectangleItem))]
    [XmlInclude(typeof(CircleItem))]
    [XmlInclude(typeof(PathItem))]
    //CUSTOM ITEMS
    [XmlInclude(typeof(PObjectTemplate))]
    [XmlInclude(typeof(RevoluteJointTemplate))]
    [XmlInclude(typeof(AngleJointTemplate))]
    [XmlInclude(typeof(SliderJointTemplate))]
    [XmlInclude(typeof(SpringTemplate))]
    [XmlInclude(typeof(PlayerStart))]
    [XmlInclude(typeof(NPCStart))]
    [XmlInclude(typeof(PointLightTemplate))]

    
    public partial class Item
    {
        /// <summary>
        /// The name of this item.
        /// </summary>
        [XmlAttribute()]
        public String Name;

        [XmlIgnore()]
        public int Id;

        /// <summary>
        /// Should this item be visible?
        /// </summary>
        /// 
        [XmlIgnore()]
        [XmlAttribute()]
        public bool Visible;

        //[XmlIgnore()]
        public string ArchetypeName;
        /// <summary>
        /// The item's Position in world space.
        /// </summary>
        /// 
       // [XmlIgnore()]
        public Vector2 PositionOrig;

        /// <summary>
        /// The list of the items in this layer.
        /// </summary>
        /// 

        /// <summary>
        /// A Dictionary containing any user-defined Properties.
        /// </summary>
        [XmlIgnore()]
        public SerializableDictionary CustomProperties;

        public CustomNode ArchetypeId;
        public CustomNode ActivatedBySecondaryObjectIds;
        public CustomNode SecondaryObjectId;
        public CustomNode ActivatesObjectWithSecondaryId;
        public CustomNode Position;
        public CustomNode Rotation;



        public Item()
        {
            ArchetypeId = new CustomNode("string");
            ArchetypeId.value = PositionOrig.X.ToString();
            Position = new CustomNode("vec2");
            Rotation = new CustomNode("float");
            ActivatedBySecondaryObjectIds = new CustomNode("string"); 
            SecondaryObjectId = new CustomNode("string"); 
            ActivatesObjectWithSecondaryId = new CustomNode("string");
            ActivatedBySecondaryObjectIds.value = "0,0,0,0";
            SecondaryObjectId.value="0";
            ActivatesObjectWithSecondaryId.value = "0";
            CustomProperties = new SerializableDictionary();
        }

        /// <summary>
        /// Called by root.FromFile(filename) on each Item after the deserialization process.
        /// Should be overriden and can be used to load anything needed by the Item (e.g. a texture).
        /// </summary>
        public virtual void load(ContentManager cm)
        {
        }

        public virtual void draw(SpriteBatch sb)
        {
        }
    }


    public partial class TextureItem : Item
    {
        /// <summary>
        /// The item's rotation in radians.
        /// </summary>
        /// 
       // [XmlIgnore()]
        public float RotationOrig;



        /// <summary>
        /// The item's scale vector.
        /// </summary>
        /// 
       // [XmlIgnore()]
        public Vector2 Scale;

        /// <summary>
        /// The color to tint the item's texture with (use white for no tint).
        /// </summary>
        /// 
        //[XmlIgnore()]
        public Color TintColor;

        /// <summary>
        /// If true, the texture is flipped horizontally when drawn.
        /// </summary>
        /// 
        //[XmlIgnore()]
        public bool FlipHorizontally;

        /// <summary>
        /// If true, the texture is flipped vertically when drawn.
        /// </summary>
        /// 
       // [XmlIgnore()]
        public bool FlipVertically;

        /// <summary>
        /// The path to the texture's filename (including the extension) relative to ContentrootFolder.
        /// </summary>
        public String texture_filename;

        /// <summary>
        /// The texture_filename without extension. For using in Content.Load<Texture2D>().
        /// </summary>
        /// 
       // [XmlIgnore()]
        public String asset_name;

        /// <summary>
        /// The XNA texture to be drawn. Can be loaded either from file (using "texture_filename") 
        /// or via the Content Pipeline (using "asset_name") - then you must ensure that the texture
        /// exists as an asset in your project.
        /// Loading is done in the Item's load() method.
        /// </summary>
        Texture2D texture;

        /// <summary>
        /// The item's origin relative to the upper left corner of the texture. Usually the middle of the texture.
        /// Used for placing and rotating the texture when drawn.
        /// </summary>
        /// 
       // [XmlIgnore()]
        public Vector2 Origin;


        [XmlIgnore()]
        public bool isTemplate;

        public TextureItem()
        {
        }

        /// <summary>
        /// Called by root.FromFile(filename) on each Item after the deserialization process.
        /// Loads all assets needed by the TextureItem, especially the Texture2D.
        /// You must provide your own implementation. However, you can rely on all public fields being
        /// filled by the root deserialization process.
        /// </summary>
        public override void load(ContentManager cm)
        {
            //throw new NotImplementedException();

            //TODO: provide your own implementation of how a TextureItem loads its assets
            //for example:
            //this.texture = Texture2D.FromFile(<GraphicsDevice>, texture_filename);
            //or by using the Content Pipeline:
            //this.texture = cm.Load<Texture2D>(asset_name);

        }

        public override void draw(SpriteBatch sb)
        {
            if (!Visible) return;
            SpriteEffects effects = SpriteEffects.None;
            if (FlipHorizontally) effects |= SpriteEffects.FlipHorizontally;
            if (FlipVertically) effects |= SpriteEffects.FlipVertically;
            sb.Draw(texture, PositionOrig, null, TintColor, RotationOrig, Origin, Scale, effects, 0);
        }
    }


    public partial class RectangleItem : Item
    {
        public float Width;
        public float Height;
        public Color FillColor;

        const float positionRatio = 200.0f;
        const float scaleRatio = 200.0f;
         public CustomNode ResetOnLoad;
        public CustomNode Scale;
        public CustomNode BodyScale;
        public CustomNode ManualBodyScale;
        public RectangleItem()
        {
            ResetOnLoad = new CustomNode("string");
            Scale = new CustomNode("string");
            BodyScale = new CustomNode("string");
            ManualBodyScale = new CustomNode("string");
            Scale.value = "";
        }
    }



    public partial class CircleItem : Item
    {
        public float Radius;
        public Color FillColor;
        public CustomNode Scale;

        const float scaleRatio = 200.0f;

        public CircleItem()
        {
            Scale = new CustomNode("string");
        }
    }


    public partial class PathItem : Item
    {
        public Vector2[] LocalPoints;
        public Vector2[] WorldPoints;
        public bool IsPolygon;
        public int LineWidth;
        public Color LineColor;

        public PathItem()
        {
        }
    }


    public partial class CustomNode
    {
        /// <summary>
        /// The name of the layer.
        /// </summary>
        [XmlAttribute()]
        public String Type;

        //[XmlAttribute()]
        //public String Description;

        //[XmlAttribute()]
        //public Type Type;
       
        [XmlElement()]
        public String value;


        /// <summary>
        /// A Dictionary containing any user-defined Properties.
        /// </summary>
        // public SerializableDictionary CustomProperties;


        public CustomNode()
        {
            Type = "string";
            value = "";
        }

        public CustomNode(string s)
        {
            Type = s;
            value = "";
        }

        public void draw(SpriteBatch sb)
        {
            //if (!Visible) return;
            //foreach (Item item in Items) item.draw(sb);
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    //    NEEDED FOR SERIALIZATION. YOU SHOULDN'T CHANGE ANYTHING BELOW!
    //
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////


    public class CustomProperty
    {
        public string name;
        public object value;
        public Type type;
        public string description;

        public CustomProperty()
        {
        }

        public CustomProperty(string n, object v, Type t, string d)
        {
            name = n;
            value = v;
            type = t;
            description = d;
        }

        public CustomProperty clone()
        {
            CustomProperty result = new CustomProperty(name, value, type, description);
            return result;
        }
    }


    public class SerializableDictionary : Dictionary<String, CustomProperty>, IXmlSerializable
    {

        public SerializableDictionary()
            : base()
        {

        }

        public SerializableDictionary(SerializableDictionary copyfrom)
            : base(copyfrom)
        {
            string[] keyscopy = new string[Keys.Count];
            Keys.CopyTo(keyscopy, 0);
            foreach (string key in keyscopy)
            {
                this[key] = this[key].clone();
            }
        }

        public System.Xml.Schema.XmlSchema GetSchema()
        {
            return null;
        }

        public void ReadXml(System.Xml.XmlReader reader)
        {

            bool wasEmpty = reader.IsEmptyElement;
            reader.Read();

            if (wasEmpty) return;

            while (reader.NodeType != System.Xml.XmlNodeType.EndElement)
            {
                CustomProperty cp = new CustomProperty();
                cp.name = reader.GetAttribute("Name");
                cp.description = reader.GetAttribute("Description");

                string type = reader.GetAttribute("Type");
               
                if (type == "string") cp.type = typeof(string);
                if (type == "bool") cp.type = typeof(bool);
                if (type == "Vector2") cp.type = typeof(Vector2);
                if (type == "Color") cp.type = typeof(Color);
                if (type == "Item") cp.type = typeof(Item);

                if (cp.type == typeof(Item))
                {
                    cp.value = reader.ReadInnerXml();
                    this.Add(cp.name, cp);
                }
                else
                {
                    reader.ReadStartElement("Property");
                    XmlSerializer valueSerializer = new XmlSerializer(cp.type);
                    object obj = valueSerializer.Deserialize(reader);
                    cp.value = Convert.ChangeType(obj, cp.type);
                    this.Add(cp.name, cp);
                    reader.ReadEndElement();
                }

                reader.MoveToContent();
            }
            reader.ReadEndElement();
        }

        public void WriteXml(System.Xml.XmlWriter writer)
        {
            foreach (String key in this.Keys)
            {
                writer.WriteStartElement("Property");
                writer.WriteAttributeString("Name", this[key].name);
                if (this[key].type == typeof(string)) writer.WriteAttributeString("Type", "string");
                if (this[key].type == typeof(bool)) writer.WriteAttributeString("Type", "bool");
                if (this[key].type == typeof(Vector2)) writer.WriteAttributeString("Type", "Vector2");
                if (this[key].type == typeof(Color)) writer.WriteAttributeString("Type", "Color");
                if (this[key].type == typeof(Item)) writer.WriteAttributeString("Type", "Item");

                writer.WriteAttributeString("Description", this[key].description);

                if (this[key].type == typeof(Item))
                {
                    Item item = (Item)this[key].value;
                    if (item != null) writer.WriteString(item.Name);
                    else writer.WriteString("$null$");
                }
                else
                {
                    XmlSerializer valueSerializer = new XmlSerializer(this[key].type);
                    valueSerializer.Serialize(writer, this[key].value);
                }
                writer.WriteEndElement();
            }
        }

        /// <summary>
        /// Must be called after all Items have been deserialized. 
        /// Restores the Item references in CustomProperties of type Item.
        /// </summary>
        public void RestoreItemAssociations(root root)
        {
            foreach (CustomProperty cp in Values)
            {
                if (cp.type == typeof(Item)) cp.value = root.getItemByName((string)cp.value);
            }
        }


    }






}