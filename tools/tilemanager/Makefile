GOAL=tilemanager.exe
SOURCES=$(wildcard *.cs)
CSFLAGS=-debug -pkg:gtk-sharp-2.0 -pkg:glade-sharp-2.0 -r:System.Drawing
RESOURCES=tiler.glade
CSFLAGS+=$(foreach file,$(RESOURCES),-resource:$(file) )

$(GOAL): $(SOURCES) $(RESOURCES)
	mcs $(CSFLAGS) $(SOURCES) -out:$(GOAL)

clean:
	rm -f $(GOAL)

.PHONY: clean
