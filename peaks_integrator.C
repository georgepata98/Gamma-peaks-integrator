{
  TH1D *h1 = new TH1D("h1", "histogram", 3000, 0., 3000.);  //nbins, x_min, x_max
  TAxis *xAxis = h1->GetXaxis();
  fstream ifile, ofile;

  Double_t y;
  Int_t x=0, j;

  cout << endl << "Choose:" << endl;
  cout << "1. Histogram visualization" << endl;
  cout << "2. Peaks integration" << endl;
  cin >> j;
  

  ifile.open("data.txt", ios::in);  //extract data from file
  while(!ifile.eof() && x<3000)
  {
    ifile >> y;
    h1->Fill(x, y);  //x, x-weight
    x++;
  }
  ifile.close();


  
  //customization----------
  if(j==1) 
  {
    TCanvas *c1 = new TCanvas("c1", "Canvas", 800, 600);

    c1->SetGrid();
    h1->SetTitle("gamma spectrum");
    h1->GetXaxis()->SetTitle("channel no.");
    h1->GetYaxis()->SetTitle("amplitude");
    h1->GetXaxis()->SetRangeUser(850, 950);  //zoom range on X axis
    h1->GetYaxis()->SetRangeUser(0., 800);  //zoom range on Y axis
    h1->Draw("hist");
  }



  //integration----------
  if(j==2) 
  {
    Double_t x1, x2, integral, binWidth;
    Int_t num, binStart, binStop;

    cout << endl << "Enter the number of peaks you want to integrate: "; cin >> num;

    ofile.open("odata.txt", ios::out);  //write data to file
    for(Int_t i=1; i<=num; i++)
    {
      cout << endl << "-----Peak_" << i << "-----" << endl;
      if(i==1) 
      {
        ofile << "Peak no. | x_1 | x_2 | Integral val." << endl;
      }
      cout << "Interval you want to integrate: " << endl;
      cout << "x_1 = "; cin >> x1;
      cout << "x_2 = "; cin >> x2;
      binWidth=(xAxis->GetXmax()-xAxis->GetXmin())/xAxis->GetNbins();  //grosimea binului = (x_max - x_min) / nbins
      binStart=x1/binWidth+1;  //numarul binului pt. valoarea x1
      binStop=x2/binWidth+1;  //numarul binului pt. valoarea x2
      integral=h1->Integral(binStart, binStop);  //integrala de la binStart la binStop
      integral=integral-(x2-x1)*(h1->GetBinContent(binStart)+h1->GetBinContent(binStop))*.5;  //scadere liniara background din integrala
      ofile << i << " " << x1 << " " << x2 << " " << integral << endl;
    }
    ofile.close();
  }

  cout << endl << "Program finished successfully!" << endl;
}