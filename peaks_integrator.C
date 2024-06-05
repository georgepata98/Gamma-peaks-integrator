{
  #include <vector>
  #include <string>

  TH1D *h1 = new TH1D("h1", "histogram", 3000, 0., 3000.);  //nbins, x_min, x_max
  TAxis *xAxis = h1->GetXaxis();
  fstream ifile, ofile;

  Double_t y;
  Int_t x=0, j;
  bool manualInput=true;  //introducere manuala a parametrilor de integrare
  TString fileName;

  cout << endl << "Choose:" << endl;
  cout << "1. Histogram visualization" << endl;
  cout << "2. Peaks integration" << endl;
  cin >> j;
  
  cout << "Name of the input file (where is the spectrum data): "; cin >> fileName;


  ifile.open(fileName+".txt", ios::in);  //extract data from txt file
  if(!ifile) {
    cout << endl << "Error: Unable to open the file!" << endl;
    return 1;
  }
  while(!ifile.eof() && x<xAxis->GetXmax())
  {
    ifile >> y;
    h1->Fill(x, y);  //x, x-weight
    x++;
  }
  ifile.close();


  
  //customization----------
  if(j==1) 
  {
    TCanvas *c1 = new TCanvas("c1", "Canvas", 1200, 600);

    c1->SetGrid();
    h1->SetTitle("Gamma spectrum");
    h1->GetXaxis()->SetTitle("channel no.");
    h1->GetYaxis()->SetTitle("amplitude");
    h1->GetXaxis()->SetRangeUser(850, 950);  //zoom range on X axis
    //h1->GetYaxis()->SetRangeUser(0, 500);  //zoom range on Y axis
    h1->Draw("hist");
  }



  //integration----------
  if(j==2) 
  {
    Double_t x1, x2, integral, binWidth, bg1, bg2, bg3, bg4;
    Int_t num, polOrder, binStart, binStop;
    TString answer;

    cout << "Do you want to manually introduce the integration parameters? (yes or no): "; cin >> answer;
    if(answer=="yes" || answer=="y") manualInput=true;
    else if(answer=="no" || answer=="n") manualInput=false;

    if(manualInput)
    {
      cout << "Name of the output file (where to write data): "; cin >> fileName;

      cout << endl << "****************************************" << endl << "Enter the number of peaks you want to integrate: "; cin >> num;


      ofile.open(fileName+".txt", ios::out);  //write data to txt file
      for(Int_t i=1; i<=num; i++)
      {
        cout << endl << "Select the order of the polynomial fit for the current peak: " << endl;
        cout << "1. First order polynomial fit" << endl;
        cout << "2. Second order polynomial fit" << endl;
        cin >> polOrder;
        cout << endl << "-----Peak_" << i << "-----" << endl;
        if(i==1) 
        {
          ofile << "Peak no. | bg_1 | bg_2 | bg_3 | bg_4 | x_1 | x_2 | Integral val. | Integral val. err." << endl;
        }
        if(polOrder==1)
        {
          cout << "Interval you want to integrate: " << endl;
          cout << "x_1 = "; cin >> x1;
          cout << "x_2 = "; cin >> x2;
          binWidth=(xAxis->GetXmax()-xAxis->GetXmin())/xAxis->GetNbins();  //grosimea binului = (x_max - x_min) / nbins
          binStart=x1/binWidth+1;  //numarul binului pt. valoarea x1
          binStop=x2/binWidth+1;  //numarul binului pt. valoarea x2
          integral=h1->Integral(binStart, binStop);  //integrala de la binStart la binStop
          integral=integral-(x2-x1)*(h1->GetBinContent(binStart)+h1->GetBinContent(binStop))*.5;  //scadere liniara background din integrala
          ofile << i << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << x1 << " " << x2 << " " << integral << " " << TMath::Sqrt(integral) << endl;
        }
        if(polOrder==2)
        {
          TGraph *gr1 = new TGraph();
          Double_t binbg1, binbg2, binbg3, binbg4;
          Int_t aux=0;

          cout << "Lefthand background margin: " << endl;
          cout << "bg_1 = "; cin >> bg1;
          cout << "bg_2 = "; cin >> bg2;
          cout << "Righthand background margin: " << endl;
          cout << "bg_3 = "; cin >> bg3;
          cout << "bg_4 = "; cin >> bg4;
          TF1 *fitFunction = new TF1("fitFunction", "pol2", bg1, bg4);  //pol2 fit function from bg1 to bg4
          cout << "Peak interval you want to integrate: " << endl;
          cout << "x_1 = "; cin >> x1;
          cout << "x_2 = "; cin >> x2;
          binWidth=(xAxis->GetXmax()-xAxis->GetXmin())/xAxis->GetNbins();
          binStart=x1/binWidth+1;
          binStop=x2/binWidth+1;
          binbg1=bg1/binWidth+1;  //numarul binului pt. valoarea bg1
          binbg2=bg2/binWidth+1;  //numarul binului pt. valoarea bg2
          binbg3=bg3/binWidth+1;  //numarul binului pt. valoarea bg3
          binbg4=bg4/binWidth+1;  //numarul binului pt. valoarea bg4
          integral=h1->Integral(binStart, binStop);
          for(Int_t k=binbg1; k<=binbg2; k++)
          {
            gr1->SetPoint(aux, k, h1->GetBinContent(k));
            aux++;
          }
          for(Int_t k=binbg3; k<=binbg4; k++)
          {
            gr1->SetPoint(aux, k, h1->GetBinContent(k));
            aux++;
          }
          gr1->Fit(fitFunction, "R");
          integral=integral-fitFunction->Integral(x1, x2);  //scadere background dat de functia de fit, pe intervalul (x1, x2)
          ofile << i << " " << bg1 << " " << bg2 << " " << bg3 << " " << bg4 << " " << x1 << " " << x2 << " " << integral << " " << TMath::Sqrt(integral) << endl;


          /*gr1->SetMarkerStyle(8);
          gr1->Draw("AP");
          fitFunction->Draw("SAME");*/
        }
      }
      ofile.close();
    }

    else if(!manualInput)
    {
      cout << "Name of the output file (where is the integration data): "; cin >> fileName;
      std::string line;
      Int_t auxx=0;

      ofile.open(fileName+".txt", ios::in);
      if(!ofile) {
        cout << endl << "Error: Unable to open the file!" << endl;
        return 1;
      }
      while(std::getline(ofile, line))
      {
        std::istringstream iss(line);
        if(auxx==0)  //jump over the first line of the file
        {
          auxx=1;
          continue;
        }
        if(line.empty()) break;
        iss >> x >> bg1 >> bg2 >> bg3 >> bg4 >> x1 >> x2 >> integral >> y;
        cout << x << " " << bg1 << " " << bg2 << " " << bg3 << " " << bg4 << " " << x1 << " " << x2 << " " << integral << " " << y << endl;
      }
      ofile.close();
    }
  }

  cout << endl << "Program finished successfully!" << endl;
}
