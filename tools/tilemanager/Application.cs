//  $Id$
using System;
using System.IO;
using System.Collections;
using Gtk;
using Gdk;
using Glade;

public class Application {
    [Glade.Widget]
    private Gtk.Window MainWindow;
    [Glade.Widget]
    private Gtk.CheckMenuItem useNewSyntax;
    [Glade.Widget]
    private Gtk.DrawingArea DrawingArea;
    [Glade.Widget]
	//Flags CheckButtons
    private Gtk.CheckButton SolidCheckButton;
    [Glade.Widget]
    private Gtk.CheckButton UniSolidCheckButton;
    [Glade.Widget]
    private Gtk.CheckButton IceCheckButton;
    [Glade.Widget]
    private Gtk.CheckButton WaterCheckButton;
    [Glade.Widget]
    private Gtk.CheckButton HurtsCheckButton;
    [Glade.Widget]
    private Gtk.CheckButton FireCheckButton;
    [Glade.Widget]
    private Gtk.CheckButton BrickCheckButton;
    [Glade.Widget]
    private Gtk.CheckButton CoinCheckButton;
    [Glade.Widget]
    private Gtk.CheckButton FullBoxCheckButton;
    [Glade.Widget]
    private Gtk.CheckButton SlopeCheckButton;
    [Glade.Widget]
    private Gtk.CheckButton GoalCheckButton;
    [Glade.Widget]
    private Gtk.CheckButton DontUseCheckButton;
    [Glade.Widget]
    private Gtk.CheckButton HiddenCheckButton;
    [Glade.Widget]

    private Gtk.Entry DataEntry;
    [Glade.Widget]
    private Gtk.Entry AnimFpsEntry;
    [Glade.Widget]
    private Gtk.Entry IDEntry;
    [Glade.Widget]
    private Gtk.TreeView TileList;
    [Glade.Widget]
    private Gtk.Combo TileGroupComboBox;
    [Glade.Widget]
    private Gtk.MenuItem AddTileGroupMenu;

    [Glade.Widget]
    private Gtk.Dialog RemapDialog;
    [Glade.Widget]
    private Gtk.SpinButton RD_spinButton;

    private string tilesetdir;
    private string tilesetfile;
    private TileSet tileset;
    private TileGroup selectedgroup;

    private Tile[] Tiles;
    private bool[] SelectionArray;
    private ArrayList Selection = new ArrayList();
    private int TilesX;
    private int TilesY;
    private bool toggling;
    private bool selecting;

    private string currentimage;
    private Gdk.Pixbuf pixbuf;

    public static int Main(string[] args) {
        Gtk.Application.Init();

        Application app = new Application();

        /* that's no proper commandlineparsing, but who'll notice... */
        if(args.Length == 1)
            app.LoadTileSet(args[0]);

        Gtk.Application.Run();
        return 0;
    }

    public Application() {
        Glade.XML gxml = new Glade.XML(null, "tiler.glade", null, null);
        gxml.Autoconnect(this);

        if(MainWindow == null || DrawingArea == null || RemapDialog == null)
            throw new Exception("some widgets not found");

        DrawingArea.AddEvents((int) Gdk.EventMask.ButtonPressMask);
        DrawingArea.AddEvents((int) Gdk.EventMask.ButtonReleaseMask);
        DrawingArea.AddEvents((int) Gdk.EventMask.ButtonMotionMask);

        MainWindow.Show();
    }

    protected void OnOpen(object o, EventArgs e) {
	FileChooserDialog fileChooser = new FileChooserDialog("Select TileSet", MainWindow, FileChooserAction.Open, new object[] {});

	fileChooser.AddButton(Gtk.Stock.Cancel, Gtk.ResponseType.Cancel);
	fileChooser.AddButton(Gtk.Stock.Ok, Gtk.ResponseType.Ok);
	fileChooser.DefaultResponse = Gtk.ResponseType.Ok;
	Gtk.FileFilter filter;
	filter = new Gtk.FileFilter();
	filter.Name = "Supertux tilesets";
	filter.AddPattern("*.strf");
	filter.AddPattern("*.stgt");
	fileChooser.AddFilter( filter );
	filter = new Gtk.FileFilter();
	filter.Name = "Supertux 0.1.x tilesets";
	filter.AddPattern("*.stgt");
	fileChooser.AddFilter( filter );
	filter = new Gtk.FileFilter();
	filter.Name = "Supertux 0.3.x tilesets";
	filter.AddPattern("*.strf");
	fileChooser.AddFilter( filter );
	Gtk.FileFilter all = new Gtk.FileFilter();
	all.Name = "All Files";
	all.AddPattern("*");
	fileChooser.AddFilter( all );
	int result = fileChooser.Run();
	fileChooser.Hide();

	if(result != (int) ResponseType.Ok)
		return;

        LoadTileSet(fileChooser.Filename);
    }

    private void LoadTileSet(string file) {
        try {
            tileset = new TileSet();
            tileset.Parse(file);
            tilesetfile = file;
            tilesetdir = new FileInfo(file).Directory.ToString();
        } catch(Exception exception) {
            ShowException(exception);
        }

        Selection.Clear();
        SelectionChanged();
        FillTileGroupComboBox();
        FillTileList();

	useNewSyntax.Active = tileset.IsNew;
	if (tileset.IsNew)
		Console.WriteLine("Warning: new syntax of 0.3.x files \"More tiles in one image\" isn't currently supported for WRITING");

    }

    protected void OnImportImage(object o, EventArgs e) {
	FileChooserDialog fileChooser = new FileChooserDialog("Select ImageFile", MainWindow, FileChooserAction.Open, new object[] {});

	fileChooser.AddButton(Gtk.Stock.Cancel, Gtk.ResponseType.Cancel);
	fileChooser.AddButton(Gtk.Stock.Ok, Gtk.ResponseType.Ok);
	fileChooser.DefaultResponse = Gtk.ResponseType.Ok;
	Gtk.FileFilter all = new Gtk.FileFilter();
	all.Name = "All Files";
	all.AddPattern("*");
	fileChooser.AddFilter( all );
	int result = fileChooser.Run();
	fileChooser.Hide();

	if(result != (int) ResponseType.Ok)
		return;

	string file = fileChooser.Filename;
	string trim = tilesetdir + "/";

	if (!file.StartsWith(trim)){
		Console.WriteLine(
			"Imported file must be located inside tileset directory");
		return;
	}

        ChangeImage(file.TrimStart(trim.ToCharArray()));

        int startid = tileset.Tiles.Count;
        for(int y = 0; y < TilesY; ++y) {
            for(int x = 0; x < TilesX; ++x) {
                int i = y*TilesX+x;
                Tile tile = new Tile();
                tile.ID = startid + i;
                ImageRegion region = new ImageRegion();
                region.ImageFile = currentimage;
                region.Region = new System.Drawing.Rectangle(x*TileSet.TILE_WIDTH, y*TileSet.TILE_HEIGHT, TileSet.TILE_WIDTH, TileSet.TILE_HEIGHT);
                tile.Images.Add(region);
                if(Tiles[i] != null) {
                    Console.WriteLine(
                            "Warning Tile in this region already existed...");
                }
                Tiles[i] = tile;
                tileset.Tiles.Add(tile);
            }
        }

        FillTileList();
    }

    private void ChangeImage(string file) {
        if(file == "") {
            currentimage = "";
            pixbuf = null;
            return;
        }
        try {
            pixbuf = new Pixbuf(tilesetdir + "/" + file);
            if(pixbuf.Width % TileSet.TILE_WIDTH != 0 || pixbuf.Height % TileSet.TILE_HEIGHT != 0)
                Console.WriteLine("Warning: Image Width or Height is not a multiple of 32");
        } catch(Exception e) {
            ShowException(e);
            return;
        }
        currentimage = new FileInfo(file).Name;
        TilesX = pixbuf.Width / TileSet.TILE_WIDTH;
        TilesY = pixbuf.Height / TileSet.TILE_HEIGHT;
        SelectionArray = new bool[TilesX * TilesY];
        Tiles = new Tile[TilesX * TilesY];

        // search tileset for tiles with matching image
        foreach(Tile tile in tileset.Tiles) {
            if(tile == null)
                continue;
            if(tile.Images.Count == 0)
                continue;
            ImageRegion region = (ImageRegion) tile.Images[0];
            if(region.ImageFile == currentimage) {
                int px = region.Region.X / TileSet.TILE_WIDTH;
                int py = region.Region.Y / TileSet.TILE_HEIGHT;
                int i = py*TilesX+px;
                if(i < 0 || i >= Tiles.Length) {
                    Console.WriteLine("Invalid Imageregion at tile " +
                            tile.ID);
                    continue;
                }
                if(Tiles[i] != null) {
                    Console.WriteLine("Multiple tiles for region " +
                            px*TileSet.TILE_WIDTH + " , " + py*TileSet.TILE_HEIGHT);
                    continue;
                }
                Tiles[i] = tile;
            }
        }

        /*   DrawingArea.Allocation
            = new Gdk.Rectangle(0, 0, pixbuf.Width, pixbuf.Height);*/
        DrawingArea.WidthRequest = pixbuf.Width;
        DrawingArea.HeightRequest = pixbuf.Height;
        DrawingArea.QueueResize();
    }

    protected void OnSave(object o, EventArgs e) {
	if (tileset.IsNew && useNewSyntax.Active) {
		MessageDialog dialog = new MessageDialog(MainWindow, DialogFlags.Modal | DialogFlags.DestroyWithParent,	MessageType.Error, ButtonsType.Ok,
								"Sorry, the file you are editing is too new and 0.3.x syntax is not supported yet.");
		dialog.Run();
		dialog.Destroy();
	} else {
		tileset.Write(tilesetfile);
	}
    }

    protected void OnQuit(object o, EventArgs e) {
        Gtk.Application.Quit();
    }

    protected void OnDeleteQuit(object o, DeleteEventArgs e) {
        Gtk.Application.Quit();
    }

    protected void OnAbout(object o, EventArgs e) {
//		string[] authors = new string[]{
//			"<autors?>",
//		};

		Gtk.AboutDialog dialog = new Gtk.AboutDialog();
//		dialog.Icon = <icon>;
		dialog.ProgramName = "SuperTux Tiler";
		dialog.Version = "0.0.3";
		dialog.Comments = "A tileset editor for SuperTux 0.1.x";
//		dialog.Authors = authors;
		dialog.Copyright = "Copyright (c) 2006 SuperTux Devel Team";
		dialog.License =
			"This program is free software; you can redistribute it and/or modify" + Environment.NewLine +
			"it under the terms of the GNU General Public License as published by" + Environment.NewLine +
			"the Free Software Foundation; either version 2 of the License, or" + Environment.NewLine +
			"(at your option) any later version." + Environment.NewLine +
			Environment.NewLine +
			"This program is distributed in the hope that it will be useful," + Environment.NewLine +
			"but WITHOUT ANY WARRANTY; without even the implied warranty of" + Environment.NewLine +
			"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the" + Environment.NewLine +
			"GNU General Public License for more details." + Environment.NewLine +
			Environment.NewLine +
			"You should have received a copy of the GNU General Public License" + Environment.NewLine +
			"along with this program; if not, write to the Free Software Foundation, Inc.," + Environment.NewLine +
			"59 Temple Place, Suite 330, Boston, MA 02111-1307 USA" + Environment.NewLine;
		dialog.Website = "http://supertux.lethargik.org/";
		dialog.WebsiteLabel = "SuperTux on the Web";
		dialog.Run();
		dialog.Destroy();
    }

    protected void OnRemapTiles(object o, EventArgs e) {
	if(Tiles == null)
		return;
	RemapDialog.Show();
    }

    protected void OnRemapDialogCancel(object o, EventArgs e) {
	RemapDialog.Hide();
    }

    protected void OnRemapDialogApply(object o, EventArgs e) {
	RemapDialog.Hide();
        try {

            // remap tiles
            int id;
            try {
                id = RD_spinButton.ValueAsInt;
            } catch(Exception exception) {
                ShowException(exception);
                return;
            }
            RemapTiles(id);
        } finally {
            RD_spinButton.Value = 1;
        }
    }

    protected void OnCreateTileGroup(object o, EventArgs e) {
    }

    protected void OnRenameTileGroup(object o, EventArgs e) {
    }

    protected void RemapTiles(int startID) {
	if(Tiles == null)
		return;

	// remap tiles
	int id = startID;
	foreach(Tile tile in Selection) {
		if(tile.ID == -1)
			continue;

		int oldid = tile.ID;
		tile.ID = id++;
		// remap in all tilegroups...
		foreach(TileGroup tilegroup in tileset.TileGroups) {
			int idx = tilegroup.Tiles.IndexOf(oldid);
			if(idx >= 0) {
				tilegroup.Tiles[idx] = tile.ID;
			}
		}
	}
	FillTileList();
	SelectionChanged();
    }

    protected void OnDrawingAreaExpose(object o, ExposeEventArgs e) {
        if(pixbuf == null)
            return;

        Drawable drawable = e.Event.Window;
        Gdk.GC gc = new Gdk.GC(drawable);
        drawable.DrawPixbuf(gc, pixbuf, 0, 0, 0, 0,
                pixbuf.Width, pixbuf.Height, RgbDither.None, 0, 0);

        gc.RgbFgColor = new Color(0xff, 0, 0);
        foreach(Tile tile in Selection) {
            System.Drawing.Rectangle rect
                = ((ImageRegion) tile.Images[0]).Region;
            drawable.DrawRectangle(gc, false, rect.X, rect.Y, rect.Width,
                    rect.Height);
        }

        e.RetVal = false;
    }

    protected void OnDrawingAreaButtonPress(object o, ButtonPressEventArgs e) {
        if(SelectionArray == null)
            return;

        selecting = true;

        for(int i = 0; i < SelectionArray.Length; ++i)
            SelectionArray[i] = false;
        select((int) e.Event.X, (int) e.Event.Y);
    }

    private void select(int x, int y) {
        int tile = y/TileSet.TILE_HEIGHT * TilesX + x/TileSet.TILE_WIDTH;
        if(tile < 0 || tile >= SelectionArray.Length)
            return;

        SelectionArray[tile] = true;
        SelectionArrayChanged();
    }

    protected void OnDrawingAreaMotionNotify(object i, MotionNotifyEventArgs e) {
        if(!selecting)
            return;
        select((int) e.Event.X, (int) e.Event.Y);
    }

    protected void OnDrawingAreaButtonRelease(object o, ButtonPressEventArgs e) {
        selecting = false;
    }

    protected void OnCheckButtonToggled(object sender, EventArgs e) {
        if(toggling)
            return;
        foreach(Tile tile in Selection) {
            if(sender == SolidCheckButton)
                tile.SetAttribute(Attribute.SOLID, SolidCheckButton.Active);
            if(sender == UniSolidCheckButton)
                tile.SetAttribute(Attribute.UNISOLID, UniSolidCheckButton.Active);
            if(sender == IceCheckButton)
                tile.SetAttribute(Attribute.ICE, IceCheckButton.Active);
            if(sender == WaterCheckButton)
                tile.SetAttribute(Attribute.WATER, WaterCheckButton.Active);
            if(sender == HurtsCheckButton)
                tile.SetAttribute(Attribute.HURTS, HurtsCheckButton.Active);
            if(sender == FireCheckButton)
                tile.SetAttribute(Attribute.FIRE, FireCheckButton.Active);
            if(sender == BrickCheckButton)
                tile.SetAttribute(Attribute.BRICK, BrickCheckButton.Active);
            if(sender == CoinCheckButton)
                tile.SetAttribute(Attribute.COIN, CoinCheckButton.Active);
            if(sender == FullBoxCheckButton)
                tile.SetAttribute(Attribute.FULLBOX, FullBoxCheckButton.Active);
            if(sender == SlopeCheckButton)
                tile.SetAttribute(Attribute.SLOPE, SlopeCheckButton.Active);
            if(sender == GoalCheckButton)
                tile.SetAttribute(Attribute.GOAL, GoalCheckButton.Active);
            if(sender == HiddenCheckButton)
                tile.Hidden = HiddenCheckButton.Active;
            if(sender == DontUseCheckButton)
                tile.ID = DontUseCheckButton.Active ? -1 : 0;
        }
    }

    protected void OnEntryChanged(object sender, EventArgs e) {
        if(toggling)
            return;
        foreach(Tile tile in Selection) {
            try {
                if(sender == IDEntry)
                    tile.ID = Int32.Parse(IDEntry.Text);
                if(sender == DataEntry)
                    tile.Data = Int32.Parse(DataEntry.Text);
                if(sender == AnimFpsEntry)
                    tile.AnimFps = Single.Parse(AnimFpsEntry.Text);
            } catch(Exception) {
                // ignore parse errors for now...
            }
        }
    }

    private void SelectionArrayChanged() {
        Selection.Clear();
        for(int i = 0; i < SelectionArray.Length; ++i) {
            if(!SelectionArray[i])
                continue;

            if(Tiles[i] == null) {
                Console.WriteLine("Tile doesn't exist yet");
                // TODO ask user to create new tile...
                continue;
            }
            Selection.Add(Tiles[i]);
        }

        SelectionChanged();
    }

    private void SelectionChanged() {
        bool first = true;
        toggling = true;
        string nextimage = "";
        foreach(Tile tile in Selection) {
            if(first) {
                SolidCheckButton.Active = tile.HasAttribute(Attribute.SOLID);
                UniSolidCheckButton.Active = tile.HasAttribute(Attribute.UNISOLID);
                IceCheckButton.Active = tile.HasAttribute(Attribute.ICE);
                WaterCheckButton.Active = tile.HasAttribute(Attribute.WATER);
                HurtsCheckButton.Active = tile.HasAttribute(Attribute.HURTS);
                FireCheckButton.Active = tile.HasAttribute(Attribute.FIRE);
                BrickCheckButton.Active = tile.HasAttribute(Attribute.BRICK);
                CoinCheckButton.Active = tile.HasAttribute(Attribute.COIN);
                FullBoxCheckButton.Active = tile.HasAttribute(Attribute.FULLBOX);
                SlopeCheckButton.Active = tile.HasAttribute(Attribute.SLOPE);
                GoalCheckButton.Active = tile.HasAttribute(Attribute.GOAL);
                HiddenCheckButton.Active = tile.Hidden;
                DontUseCheckButton.Active = tile.ID == -1;
                DataEntry.Text = tile.Data.ToString();
                AnimFpsEntry.Text = tile.AnimFps.ToString();
                IDEntry.Text = tile.ID.ToString();
                IDEntry.IsEditable = true;
                first = false;

                if(tile.Images.Count > 0) {
                    nextimage = ((ImageRegion) tile.Images[0]).ImageFile;
                }
            } else {
                IDEntry.Text += "," + tile.ID.ToString();
                IDEntry.IsEditable = false;
                if(tile.Images.Count > 0
                        && ((ImageRegion) tile.Images[0]).ImageFile != nextimage) {
                    nextimage = "";
                    pixbuf = null;
                }
            }
        }
        if(nextimage != currentimage)
            ChangeImage(nextimage);
        toggling = false;
        DrawingArea.QueueDraw();
    }

    private void FillTileList() {
        TileList.HeadersVisible = true;
        if(TileList.Columns.Length == 0)
            TileList.AppendColumn("Tile", new CellRendererText(), "text", 0);

        ListStore store = new ListStore(typeof(string));

        if(selectedgroup == null) {
            foreach(Tile tile in tileset.Tiles) {
                if(tile == null)
                    continue;
                store.AppendValues(new object[] { tile.ID.ToString() });
            }
        } else {
            foreach(int id in selectedgroup.Tiles) {
		Tile tile;
		if (id >= tileset.Tiles.Count){
			Console.WriteLine("Tile ID is above Tiles.Count: " + id.ToString());
			continue;
		} else {
			tile = (Tile) tileset.Tiles[id];
		}
                if(tile == null) {
                    Console.WriteLine("tilegroup contains deleted tile");
                    continue;
                }
                store.AppendValues(new object[] { id.ToString() });
            }
        }

        TileList.Model = store;
        TileList.Selection.Mode = SelectionMode.Multiple;
    }

    private void FillTileGroupComboBox() {
        string[] groups = new string[tileset.TileGroups.Count+1];
        groups[0] = "All";

        //Submenu submenu = new Submenu();
        for(int i = 0; i < tileset.TileGroups.Count; ++i) {
            String tilegroup = ((TileGroup) tileset.TileGroups[i]).Name;
            groups[i+1] = tilegroup;
            //submenu.Add(new MenuItem(tilegroup));
        }
        TileGroupComboBox.PopdownStrings = groups;
        TileGroupComboBox.Entry.IsEditable = false;

        //AddTileGroupMenu.Submenu = submenu;
    }

    protected void OnTileGroupComboBoxEntryActivated(object o, EventArgs args) {
        if(TileGroupComboBox.Entry.Text == "All") {
            selectedgroup = null;
        } else {
            foreach(TileGroup tilegroup in tileset.TileGroups) {
                if(tilegroup.Name == TileGroupComboBox.Entry.Text) {
                    selectedgroup = tilegroup;
                    break;
                }
            }
        }
        FillTileList();
    }

    protected void OnTileListCursorChanged(object sender, EventArgs e) {
        TreeModel model;
        TreePath[] selectpaths =
            TileList.Selection.GetSelectedRows(out model);

        Selection.Clear();
        foreach(TreePath path in selectpaths) {
            TreeIter iter;
            model.GetIter(out iter, path);
            int id = Int32.Parse(model.GetValue(iter, 0).ToString());
            Selection.Add(tileset.Tiles[id]);
        }
        SelectionChanged();
    }

    private void ShowException(Exception e) {
        MessageDialog dialog = new MessageDialog(MainWindow,
                DialogFlags.Modal | DialogFlags.DestroyWithParent,
                MessageType.Error, ButtonsType.Ok,
                e.Message);
        dialog.Run();
        dialog.Destroy();
    }
}
