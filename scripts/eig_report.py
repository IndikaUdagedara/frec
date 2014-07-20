#!/usr/bin/python

import sys;
import os;
import glob;
import string;
import imp;
import shutil;
import re;


###############################################################
### set up

if len(sys.argv) != 3:
	print "usage:"
	print "%s <config file> <output dir>" % sys.argv[0];
	sys.exit(1);

OutPath = sys.argv[2];
ConfigFile = OutPath + "/" + sys.argv[1];

if os.path.exists(OutPath) == True:
	shutil.rmtree(OutPath);

os.mkdir(OutPath);
shutil.copy(sys.argv[1], ConfigFile);

imp.load_source('Conf', ConfigFile);
import Conf;

ImgDir = OutPath + "/" + os.path.basename(Conf.OutImgDir);
shutil.copytree(Conf.OutImgDir, ImgDir);




###############################################################
### read img names

Pattern = Conf.OutImgDir + "/*.jpg";
print Pattern;
ImgList = glob.glob(Pattern);
Imgs = {};
for i in ImgList:
	j = os.path.basename(i).split("_");
	if Imgs.has_key(j[0]) == False:
		Imgs[j[0]] = [];

	Imgs[j[0]].append(os.path.basename(i));
	
f = open(OutPath + "/db.html", 'w+');
f.write("<html>\n");
for i, j in Imgs.iteritems():
	f.write("Person: " + i + "<br>\n");
	for k in j:
		img = "<img src=\"./SavedImages/%s\">" % k;
		f.write(img);
	f.write("<br><br>");
f.write("</html>\n");
f.close();



###############################################################
### write self test data in html
def PlotPNG(InFile, OutFile, Src, Dst, Tmp):

	TmpFile = open(Tmp, 'w+');
	TmpFile.truncate();
	TmpFile.seek(0, 0);
	TmpFile.write("set term png medium\n");
	TmpFile.write("set output \'%s/%s\'\n" % (os.path.abspath(Dst), OutFile));
	TmpFile.write("set xr[0:6]\n");
	TmpFile.write("load \'%s\'\n" % (InFile));
	TmpFile.flush();
	Cwd = os.getcwd();
	os.chdir(Src);
	os.system("gnuplot %s/%s" % (Cwd, TmpFile.name));
	os.chdir(Cwd);
	TmpFile.close();



def WriteHtml(Id, Protected):
	PerHtml = open(OutPath + "/Person_" + Id + ".html", 'w+');
	PerHtml.write("Self Test for Person: %s<br>" % Id);
	PerHtml.write("<br><br>")
	PerHtml.write("<a href=\"./results.html\">HOME</a>");
	PerHtml.write("<br><br>")
	PerHtml.write("<br><br>")

	Tmp = OutPath + "/tmp.p";
	
	for i in Protected:
		fDir = Conf.EigenMethod_DataDir + "/SelfTest_" + Id + "/Face_" +  i[1];
		if os.path.exists(fDir):

			PerHtml.write("<a href=\"./Face_%s_%s.html\"> Face %s </a><br>" % (Id, i[1], i[1]));

			FaceHtml = open("%s/Face_%s_%s.html" % (OutPath, Id, i[1]), 'w+');
			FaceHtml.write("<a href=\"./Person_%s.html\"> Person %s </a><br>" % (Id, Id) );
			FaceHtml.write("<a href=\"./SavedImages/%s\"> Tested Image </a><br>" % (i[0]));
			FaceHtml.write("<br><br>");
			FaceHtml.write("<br><br>");

			avgImg = "avg_%s_%s.png" % (Id, i[1]);
			PlotPNG("person_avg.p", avgImg, fDir, OutPath, Tmp);
			FaceHtml.write("Average of persons\n");
			FaceHtml.write("<br>");
			FaceHtml.write("<img src=\"./%s\"></img>" % avgImg);
			FaceHtml.write("<br>");

			avgWithInput = glob.glob(Conf.EigenMethod_DataDir + "/SelfTest_%s/Face_%s/avg_with_input*" % (Id, i[1]));
			if len(avgWithInput) > 0:
				avgWithInputImg = "avg_input_%s_%s.png" % (Id, i[1]);
				#print os.path.basename(avgWithInput[0]);
				PlotPNG(os.path.basename(avgWithInput[0]), avgWithInputImg, fDir, OutPath, Tmp);
				FaceHtml.write("Average with Input Image\n");
				FaceHtml.write("<br>");
				FaceHtml.write("<img src=\"./%s\"></img>" % avgWithInputImg);
				FaceHtml.write("<br>");

			
		FaceHtml.close();
			
		
	PerHtml.close();




###############################################################
### read self test info

Pattern = Conf.EigenMethod_DataDir + "/SelfTest_*";
PersonDir = glob.glob(Pattern);

ResultsHtml = open(OutPath + "/results.html", 'w+');

ResultsHtml.write('#'*30 + "<br>");
ResultsHtml.write("Results<br>");
ResultsHtml.write('#'*30 + "<br>");
ResultsHtml.write("<br><br>");
ResultsHtml.write("<a href=\"./db.html\">Database</a>");
ResultsHtml.write("<br><br>");


for i in PersonDir:
	BaseDir = os.path.basename(i);
	Id = BaseDir.split("_")[1];
	f = open(i + "/SelfTest." + Id + ".log");

	Protected = [];
	for line in f:
		j = re.search("Protected", line);
		if j != None:
			k = string.split(string.rstrip(line, '\n'), ":");
			k = map(string.rstrip, k);
			k = map(string.lstrip, k);
			Protected.append(k[1:]);
	
	f.close();

	WriteHtml(Id, Protected)	
	ResultsHtml.write("<a href=\"./Person_%s.html\">Person %s</a>" % (Id, Id));
	ResultsHtml.write("<br>");



ResultsHtml.close();		


