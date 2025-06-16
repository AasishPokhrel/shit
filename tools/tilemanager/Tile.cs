//  $Id$
using System;
using System.Collections;
using System.IO;
using System.Drawing;
using Lisp;

public class ImageRegion {
    public String ImageFile;
    public Rectangle Region;
}

public class Attribute {
	/// <summary>solid tile that is indestructible by Tux</summary>
	public const int SOLID     = 0x0001;
	/// <summary>uni-directional solid tile</summary>
	public const int UNISOLID  = 0x0002;
	/// <summary>a brick that can be destroyed by jumping under it</summary>
	public const int BRICK     = 0x0004;
	/// <summary>the level should be finished when touching a goaltile.</summary>
	/// <remarks>
	/// if <see cref="Data">data</see> is 0 then the endsequence should be
	/// triggered, if <see cref="Data">data</see> is 1 then we can finish
	/// the level instantly.
	/// </remarks>
	public const int GOAL      = 0x0008;
	/// <summary>slope tile</summary>
	public const int SLOPE     = 0x0010;
	/// <summary>Bonusbox, content is stored in <see cref="Data">data</see></summary>
	public const int FULLBOX   = 0x0020;
	/// <summary>Tile is a coin</summary>
	public const int COIN      = 0x0040;
	/// <summary>an ice brick that makes tux sliding more than usual</summary>
	public const int ICE       = 0x0100;
	/// <summary>a water tile in which tux starts to swim</summary>
	public const int WATER     = 0x0200;
	/// <summary>a tile that hurts the player if he touches it</summary>
	public const int HURTS     = 0x0400;
	/// <summary>for lava: WATER, HURTS, FIRE</summary>
	public const int FIRE      = 0x0800;


	// TODO: Find out why are worldmap tile attributes stored in data(s)
	// worldmap flags
	public const int WORLDMAP_NORTH = 0x0001;
	public const int WORLDMAP_SOUTH = 0x0002;
	public const int WORLDMAP_EAST  = 0x0004;
	public const int WORLDMAP_WEST  = 0x0008;

	public const int WORLDMAP_STOP  = 0x0010;
}

public class Tile {
	public int ID;
	public bool Hidden;
	public int NextTile;
	public int Attributes;
	public int Data;
	public float AnimFps;
	public string OneWayString;
	public ArrayList Images = new ArrayList();
	public ArrayList EditorImages = new ArrayList();

	public Tile() {
		ID = -1;
		NextTile = -1;
		AnimFps = 1;
	}

	public bool HasAttribute (int Attrib)
	{
		return (Attributes & Attrib) != 0;
	}

	public void SetAttribute (int Attrib, bool Value)
	{
		if (Value)
			Attributes |= Attrib;
		else
			Attributes &= (~Attrib);	//NOTE: "~" stands for bitwise negation
	}

	public bool HasWMAttribute (int Attrib)
	{
		return (Data & Attrib) != 0;
	}

	public void SetWMAttribute (int Attrib, bool Value)
	{
		if (Value)
			Data |= Attrib;
		else
			Data &= (~Attrib);	//NOTE: "~" stands for bitwise negation
	}

    public void Write(LispWriter writer) {
        writer.StartList("tile");
        writer.Write("id", ID);

	WriteTileImages(writer, "images", Images);

        if(HasAttribute(Attribute.SOLID))
            writer.Write("solid", true);
        if(HasAttribute(Attribute.UNISOLID))
            writer.Write("unisolid", true);
        if(HasAttribute(Attribute.ICE))
            writer.Write("ice", true);
        if(HasAttribute(Attribute.WATER))
            writer.Write("water", true);
        if(HasAttribute(Attribute.SLOPE))
            writer.Write("slope-type", Data);
        if(HasAttribute(Attribute.HURTS))
            writer.Write("hurts", true);
        if(HasAttribute(Attribute.FIRE))
            writer.Write("fire", true);
        if(HasAttribute(Attribute.COIN))
            writer.Write("coin", true);
        if(HasAttribute(Attribute.FULLBOX))
            writer.Write("fullbox", true);
        if(HasAttribute(Attribute.BRICK))
            writer.Write("brick", true);
        if(HasAttribute(Attribute.GOAL))
            writer.Write("goal", true);

        if(Hidden)
            writer.Write("hidden", true);
        if(NextTile >= 0)
            writer.Write("next-tile", NextTile);
        if(EditorImages != null)
            WriteTileImages(writer, "editor-images", EditorImages);
        if(Data != 0)
            writer.Write("data", Data);
        if(Images.Count > 1) {
            if(AnimFps == 1.0)
              AnimFps = 40;
            writer.Write("anim-fps", AnimFps);
        }
        if(!String.IsNullOrEmpty(OneWayString)) {
		writer.Write("one-way", OneWayString);
	}
        writer.EndList("tile");
    }

    public void Parse(Lisp.Parser parser) {
        int d = parser.Depth;
        while(parser.Parse() && parser.Depth >= d) {
            if(parser.Depth == d+1) {
                if(parser.Type != Parser.LispType.SYMBOL)
                    throw new Exception("expected SYMBOL at single tile deserialization level, but found \"" + parser.StringValue + "\"");
                string symbol = parser.SymbolValue;
                parser.Parse();
                switch(symbol) {
                    case "id":
                        ID = parser.IntegerValue;
                    break;
                    case "images":
                        ParseTileImages(parser, Images);
                        break;
                    case "editor-images":
                        ParseTileImages(parser, EditorImages);
                         break;
                    case "anim-fps":
                        AnimFps = parser.FloatValue;
                        break;
                    case "one-way":
                        OneWayString = parser.StringValue;
                        break;
                    case "data":
                        Data = parser.IntegerValue;
                        break;
                    case "next-tile":
                        NextTile = parser.IntegerValue;
                        break;
                    case "hidden":
                        Hidden = parser.BoolValue;
                        break;
                    case "solid":
                        SetAttribute(Attribute.SOLID, parser.BoolValue);
                        break;
                    case "unisolid":
                        SetAttribute(Attribute.UNISOLID, parser.BoolValue);
                        break;
                    case "ice":
                        SetAttribute(Attribute.ICE, parser.BoolValue);
                        break;
                    case "water":
                        SetAttribute(Attribute.WATER, parser.BoolValue);
                        break;
                    case "slope-type":
                        SetAttribute(Attribute.SLOPE, true);
                        Data = parser.IntegerValue;
                        break;
                    case "hurts":
                        SetAttribute(Attribute.HURTS, parser.BoolValue);
                        break;
                    case "fire":
                        SetAttribute(Attribute.FIRE, parser.BoolValue);
                        break;
                    case "brick":
                        SetAttribute(Attribute.BRICK, parser.BoolValue);
                        break;
                    case "fullbox":
                        SetAttribute(Attribute.FULLBOX, parser.BoolValue);
                        break;
                    case "coin":
                        SetAttribute(Attribute.COIN, parser.BoolValue);
                        break;
                    case "goal":
                        SetAttribute(Attribute.GOAL, parser.BoolValue);
                        break;

		//Worldmap attributes section - these are stored in Data
                    case "north":
                        SetWMAttribute(Attribute.WORLDMAP_NORTH, parser.BoolValue);
                        break;
                    case "south":
                        SetWMAttribute(Attribute.WORLDMAP_SOUTH, parser.BoolValue);
                        break;
                    case "west":
                        SetWMAttribute(Attribute.WORLDMAP_WEST, parser.BoolValue);
                        break;
                    case "east":
                        SetWMAttribute(Attribute.WORLDMAP_EAST, parser.BoolValue);
                        break;
                    case "stop":
                        SetWMAttribute(Attribute.WORLDMAP_STOP, parser.BoolValue);
                        break;
                    default:
                        Console.WriteLine("Unknown tile element " + symbol);
                        break;
                }
            }
        }
    }

    private void ParseTileImages(Lisp.Parser parser, ArrayList ImagesList) {
        if(parser.Type == Parser.LispType.END_LIST)
            return;

        int d = parser.Depth;
        do {
            ImageRegion region = new ImageRegion();
            if(parser.Type == Parser.LispType.STRING) {
                region.ImageFile = parser.StringValue;
            } else if(parser.Type == Parser.LispType.START_LIST) {
                ParseImageRegion(parser, region);
            } else {
                throw new Exception("unexpected lisp data: " + parser.Type);
            }
            ImagesList.Add(region);
        } while(parser.Parse() && parser.Depth >= d);
    }

    private void WriteTileImages(LispWriter writer, string ListName, ArrayList ImagesList) {
        if(ImagesList.Count > 0) {
            writer.StartList(ListName);
            foreach(ImageRegion region in ImagesList) {
                if(region.Region.Width != 0) {
                    writer.WriteVerbatimLine(
                            String.Format("(region \"{0}\" {1} {2} {3} {4})",
                                region.ImageFile, region.Region.Left,
                                region.Region.Top, region.Region.Width,
                                region.Region.Height));
                } else {
                    writer.WriteVerbatimLine(
                            "\"" + region.ImageFile + "\"");
                }
            }
            writer.EndList(ListName);
        } else {
            Console.WriteLine("no images on tile " + ID);
        }
    }

    private void ParseImageRegion(Lisp.Parser parser, ImageRegion region) {
        parser.Parse();
        if(parser.Type != Parser.LispType.SYMBOL)
            throw new Exception("expected symbol");
        if(parser.SymbolValue != "region")
            throw new Exception("expected region symbol");
        parser.Parse();
        if(parser.Type != Parser.LispType.STRING)
            throw new Exception("expected string");
        region.ImageFile = parser.StringValue;

        parser.Parse();
        if(parser.Type != Parser.LispType.INTEGER)
            throw new Exception("expected integer");
        region.Region.X = parser.IntegerValue;

        parser.Parse();
        if(parser.Type != Parser.LispType.INTEGER)
            throw new Exception("expected integer");
        region.Region.Y = parser.IntegerValue;

        parser.Parse();
        if(parser.Type != Parser.LispType.INTEGER)
            throw new Exception("expected integer");
        region.Region.Width = parser.IntegerValue;

        parser.Parse();
        if(parser.Type != Parser.LispType.INTEGER)
            throw new Exception("expected integer");
        region.Region.Height = parser.IntegerValue;

        parser.Parse();
        if(parser.Type != Parser.LispType.END_LIST)
            throw new Exception("expected END_LIST");
    }
}
