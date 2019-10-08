# Rnuplot
---

rnuplot is a tool for plotting ROOT data with Gnuplot. It provides a streamlined workflow and useful analysis tools.

### Example usage:
Rnuplot uses xml files as scripts.

```xml
<config>
	<!-- Specifies the ROOT file -->
	<TFile name="d" url="DATA.root" />
	
	<!-- Convert Histograms to gnuplot friendly format -->
	<Data name="h1d" from="d/histogram_name" local="true" format="x y xlow xhigh dy"/>
	<Data name="h2d" from="d/histogram2D_name" local="true" format="x y z"/>


	<!-- Generate a plot with gnuplot 
		Notice that the data can be accessed by the name given above
	-->
	<Gnuplot script='
		set terminal png size 1200,1200 font "Helvetica Light, 26"
		set output "datafiles0.png"
		load "viridis.pal"
		set logscale cb
		set xlabel "wow -> wow" font "Fira Code, 46"
		plot h2d u 1:2:($3 <= 1 ? 1/0 : $3) with image
		' />

	<!-- This one reads the gnuplot script in hello-plot.gnuplot -->
	<Gnuplot script="hello-plot.gnuplot" />

	<!-- This one reads from the XML node body. ">, <, &" etc. need to be escaped here  -->
	<Gnuplot>
		set terminal epslatex standalone 
		set output "figure.tex"
		plot h1 w histeps t "Histogram"
	</Gnuplot>

	<!-- This will run pdflatex on the figure produced above -->
	<!-- will produce figure.pdf with all latex typeset on the figure -->
	<Latex from="figure.tex" />

</config>
```


### XML Nodes

#### TFile
Opens a ROOT file in read mode.
- **url** : file to open
- **name** : a name for refering to this file

#### Data
Converts a ROOT object (TH1 etc.) to a gnuplot-friendly data file
- **name** : name for the data file. You can refer to it by this name in your gnuplot script.
- **from** : the name of the ROOT object to read from. should be "file_name/object_name".
- **format** : (Optional) Format for conversion. Available are x, y, z, xlow, xhigh, ylow, yhigh, zlow, zhigh, dx, dy, dz
- **local** : (Optional) If set to `true` the data file is written to the local directory with the name <name>.dat (name provided by attribute)

#### Gnuplot
Runs a gnuplot session. The gnuplot script can be provided in one of 3 ways (in order of priority):
1. If the **script** attribute is a valid file (exists and readable) then it will load the file
2. If the **script** attribute is given, but not a valid file, its lines will be used as the gnuplot script.
3. The body of the Gnuplot node is used. This mode requires escaping of XML characters (>,<, &, etc.) so it is not convenient.

Attributes:
- **script** : (Optional) name of file or directly script lines to send to gnuplot.
- **pause** : (Optional) whether or not to pause and drop into gnuplot terminal after plotting. This is useful for interactive work. If the terminal is set to qt, this allows a normal gnuplot session with the ROOT data available. TODO, improve pass through terminal support.

#### Latex
Run pdflatex to convert epslatex output to a final figure
Should be used with: `set terminal epslatex standalone`. The `standalone` setting is required.
Attributes:
- **from** : The name of the **.tex** figure file to convert.
- **clean** : (Optional) set to false to prevent clean up of latex files and logs


### Building


### External Dependencies 
- ROOT : https://root.cern.ch/

### Libraries and tools
- Gnuplot iostream interface for c++ (https://code.google.com/archive/p/gnuplot-cpp/)
