#include<iostream>
#include<fstream>
#include<string>


/*
this method is used to define a part (almost all) of the .geo file
and the Device part of the .tib file

!!!The only things that are needed to be set are 'there_is_overlap' and n_well
*/
int main() {

  //!!!  SETUP THESE
  bool there_is_overlap = true;
  int n_well = 50;
  int number_of_single_dot_states = 20;
  //!!!

//--------------------------------------------------------------------//

  int number_of_points = 0;  //this will be defined later

  std::string directory;
  std::string title = std::to_string(n_well) + "_wells";

  if (there_is_overlap)
  {
    title += "_with_overlap";
    directory = "output_files_with_overlap/";
  }

  else
  {
    title += "_no_overlap";
    directory = "output_files_NO_overlap/";
  }


  //------------------------------------------------------------------------------------//
  //START FILE .geo!!!

  std::ofstream MyFile(directory + title + ".geo");

  std::ifstream file_geo_part_1("input_files/input_geo_part_1.geo");
  std::string myText;

  while (getline(file_geo_part_1, myText))
  {
    MyFile<<myText<<"\n";
  }
  file_geo_part_1.close();

  for (int i = 0; i < n_well+1; ++i)
  {
    if (i == 0)
    {
      MyFile<<"barrier_0 = barrier_width;\n\n";
      MyFile<<"well_0 = barrier_0 + well_width;\n\n";
      MyFile<<"barrier_0_right = well_0 + barrier_width;\n\n";
      MyFile<<"buffer = barrier_0_right + buffer;\n\n";

    }
    else if (i == 1)
    {
      MyFile<<"barrier_1 = buffer + barrier_width;\n\n";
      MyFile<<"well_1 = barrier_1 + well_width;\n\n";
      MyFile<<"barrier_2 = well_1 + barrier_width - overlap;\n\n";
    }
    else
    {
      if (there_is_overlap)
        MyFile<<"barrier_"<<i*2 - 1<<" = barrier_"<<i*2 - 2<<" + barrier_width - overlap;\n\n";
      else
        MyFile<<"barrier_"<<i*2 - 1<<" = barrier_"<<i*2 - 2<<" + barrier_width;\n\n";

      MyFile<<"well_"<<i<<" = barrier_"<<i*2 - 1<<" + well_width;\n\n";

      if (there_is_overlap)
        if (i == n_well)
          MyFile<<"barrier_"<<i*2<<" = well_"<<i<<" + barrier_width;\n\n";
        else
          MyFile<<"barrier_"<<i*2<<" = well_"<<i<<" + barrier_width - overlap;\n\n";
      else
        MyFile<<"barrier_"<<i*2<<" = well_"<<i<<" + barrier_width;\n\n";
    }
  }

  MyFile<<"\n\n"<<"\n\n";

  int last_point = 0;
  int ctr = 0;
  for (int i = 0; i < n_well + 2; ++i)
  {
    if (i == 0)
      MyFile<<"Point(1) = {0, 0, 0, lc};\n\n";

    else if (i == 1)
    {
      MyFile<<"Point("<<2<<") = {barrier_0, 0, 0, lc};\n\n"
      <<"Point("<<3<<") = {well_0, 0, 0, lc};\n\n"
      <<"Point("<<4<<") = {barrier_0_right, 0, 0, lc};\n\n"
      <<"Point("<<5<<") = {buffer, 0, 0, lc};\n\n";

    }
    else
    {

      MyFile<<"Point("<<i*3<<") = {barrier_"<<i - 1 + ctr<<", 0, 0, lc};\n\n"
          <<"Point("<<i*3 + 1<<") = {well_"<<i - 1<<", 0, 0, lc};\n\n"
          <<"Point("<<i*3 + 2<<") = {barrier_"<<i + ctr<<", 0, 0, lc};\n\n";

      number_of_points = i*3 + 2;
      if (i == n_well-1)
        last_point = i*3 + 2;

      ctr++;
    }
  }

  MyFile<<"\n\n"<<"\n\n";

  for (int i = 0; i < number_of_points - 1; ++i)
  {
    MyFile<<"Line ("<<i + 1<<") = {"<<i + 1 <<", "<<i + 2 <<"};\n\n";
  }

  MyFile<<"\n\n"<<"\n\n";

  MyFile<<"Physical Point (\"anode\") = {1};\n\n"
        <<"Physical Point (\"cathode\") = {"<<number_of_points<<"};\n\n"
        <<"Physical Point (\"cathode_single_well\") = {4};\n\n";

  MyFile<<"\n\n";

  for (int i = 0; i < (number_of_points / 3) ; ++i)
  {
    if (i == 0)
    {
      MyFile<<"Physical Line (\"barrier"<<i*2 + 1<<"\") = {"<<i*3+1 <<"};\n\n"
            <<"Physical Line (\"well"<<i + 1<<"\") = {"<<i*3+2 <<"};\n\n"
            <<"Physical Line (\"barrier"<<i*2 + 2<<"\") = {"<<i*3+3 <<"};\n\n"
            <<"Physical Line (\"buffer\") = {"<<i*3+4 <<"};\n\n";
    }
    else
    {
      MyFile<<"Physical Line (\"barrier"<<i*2 + 1<<"\") = {"<<i*3+2 <<"};\n\n"
            <<"Physical Line (\"well"<<i + 1<<"\") = {"<<i*3+3 <<"};\n\n"
            <<"Physical Line (\"barrier"<<i*2 + 2<<"\") = {"<<i*3+4 <<"};\n\n";
    }

  }

  MyFile.close();


//------------------------------------------------------------------//
//START FILE .tib!!!

  std::ofstream MyFile2(directory + title + ".tib");

  std::ifstream MyFile3("input_files/well.txt");
  std::ifstream MyFile4("input_files/barrier.txt");
  std::ifstream MyFile5("input_files/input_tib_part_1.tib");


  std::string string;
  string = "meshfile =";
  while (getline(MyFile5, myText))
  {
    if (myText.find(string) != std::string::npos)
      myText.replace(myText.find(string), string.length(), "meshfile = " + title + ".msh");

    MyFile2<<myText<<"\n";
  }
  MyFile5.close();

  for (int i = 0; i < (number_of_points) / 3 ; ++i)
  {
    MyFile2<<"\n"<<"  Region barrier"<<i*2 + 1<<"\n";
    MyFile4.clear();
    MyFile4.seekg(0,std::ios::beg);
    while (getline(MyFile4, myText))
    {
      MyFile2<<"  "<<myText<<"\n";
    }

    MyFile2<<"\n"<<"  Region well"<<i + 1<<"\n";
    MyFile3.clear();
    MyFile3.seekg(0,std::ios::beg);
    while (getline(MyFile3, myText))
    {
      MyFile2<<"  "<<myText<<"\n";
    }

    MyFile2<<"\n"<<"  Region barrier"<<i*2 + 2<<"\n";
    MyFile4.clear();
    MyFile4.seekg(0,std::ios::beg);
    while (getline(MyFile4, myText))
    {
      MyFile2<<"  "<<myText<<"\n";
    }
  }
  MyFile2<<"\n";
  MyFile2<<"  Cluster QW_1_single\n"
        <<"  {\n"
        <<"    regions = (barrier1, well1, barrier2)\n"
        <<"  }\n\n";
  for (int i = 0; i < n_well; ++i)
  {
    MyFile2<<"  Cluster QW_"<<i + 1<<"\n"
           <<"  {\n"
           <<"    regions = (barrier"<<i*2 + 3<<", well"<<i + 2<<",  barrier"<<i*2 + 4<<")\n"
           <<"  }\n\n";
  }


  MyFile2<<"  Cluster Quantum_MQW\n";
  MyFile2<<"  {\n";
  for (int i = 0; i < n_well; ++i)
  {
    std::string mystring, mystring2;

    if (i == 0)
      mystring2 = "    regions = (";

    mystring = "QW_" + std::to_string(i + 1) + ", ";
    mystring2 += mystring;

    if (i == n_well - 1)
    {
      mystring2.erase(mystring2.end()-2, mystring2.end());
      mystring2 += ")";
    }

    MyFile2<<mystring2;
  }

  MyFile2<<"\n"<<"  }\n"<<"}\n";

  MyFile3.close();
  MyFile4.close();


  std::ifstream MyFile6("input_files/input_tib_part_2.tib");
  string = "number_of_dots =";
  std::string string2 = "num_electron_states_multi_well";
  std::string string3 = "num_electron_states_single_well";
  while (getline(MyFile6, myText))
  {
  if (myText.find(string3) != std::string::npos)
      myText.replace(myText.find(string3), string3.length(), "num_electron_states = "+ std::to_string(number_of_single_dot_states - 1));

    if (myText.find(string2) != std::string::npos)
      myText.replace(myText.find(string2), string2.length(), "num_electron_states = "+ std::to_string(n_well * number_of_single_dot_states));

    if (myText.find(string) != std::string::npos)
      myText.replace(myText.find(string), string.length(), "number_of_dots = "+ std::to_string(n_well));

    MyFile2<<myText<<"\n";
  }
  MyFile6.close();
  MyFile2.close();
}
