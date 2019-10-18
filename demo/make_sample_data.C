

void make_sample_data(){

	TFile *f = new TFile( "sample_data.root", "RECREATE" );

	TH1 * h1 = new TH1F( "h1", "", 100, -10, 10 );
	TH1 * h2 = new TH1F( "h2", "", 100, -10, 10 );

	TF1 * fgaus = new TF1( "fgaus", "gaus" );
	fgaus->SetParameters( 1, 3, 5 );
	h1->FillRandom( "fgaus", 10000 );

	h1->Draw("");

	TF1 * flandau = new TF1( "flandau", "landau" );
	flandau->SetParameters( 2, -4, 1.5 );
	h2->FillRandom( "flandau", 10000 );

	h2->Draw("");

	TH1 * hSum = (TH1*)h1->Clone("hSum");

	hSum->Add( h2 );
	hSum->Draw();

	float bins[] = {0.00, 1.00, 2.00, 3.00, 4.00, 5.00, 6.00, 7.00, 8.00, 9.00, 10.00, 12.00, 14.00, 16.00, 18.00, 20.00, 24.00, 28.00, 32.00, 36.00, 40.00, 50.00, 60.00, 70.00, 80.00, 90.00};

	TH2F *h3 = new TH2F("h3","",100,-10.,10.,100,-10.,10.); 
	TH2F *h5 = new TH2F("h5","",25,bins,25, bins); 
	TF2 *xyg = new TF2("xyg","xygaus",-10,10,-10,10); 
	for ( int i = 0; i < 10; i++ ){
		xyg->SetParameters(1,gRandom->Rndm() * 15 - 7.5, gRandom->Rndm() * 4 + 0.5, gRandom->Rndm() * 15 - 7.5, gRandom->Rndm() * 4 + 0.5); //amplitude, meanx,sigmax,meany,sigmay 
		h3->FillRandom("xyg", 10000); 

		xyg->SetParameters(1,gRandom->Rndm() * 100, gRandom->Rndm() * 20 + 5, gRandom->Rndm() * 100, gRandom->Rndm() * 30 + 5); //amplitude, meanx,sigmax,meany,sigmay 
		h5->FillRandom("xyg", 10000); 
	}
	h3->Draw("colz");


	
	TH1 * h4 = new TH1F( "h4", "", 25, bins );
	fgaus->SetParameters( 1, 3, 5 );
	h4->FillRandom( "fgaus", 10000 );
	fgaus->SetParameters( 1, 30, 60 );
	h4->FillRandom( "fgaus", 10000 );
	h4->Draw();

	h5->Draw("colz");


	f->Write();

}