

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


	TH2F *h3 = new TH2F("h3","",100,-10.,10.,100,-10.,10.); 
	TF2 *xyg = new TF2("xyg","xygaus",-10,10,-10,10); 
	for ( int i = 0; i < 10; i++ ){
		xyg->SetParameters(1,gRandom->Rndm() * 15 - 7.5, gRandom->Rndm() * 4 + 0.5, gRandom->Rndm() * 15 - 7.5, gRandom->Rndm() * 4 + 0.5); //amplitude, meanx,sigmax,meany,sigmay 
		h3->FillRandom("xyg", 10000); 
	}
	h3->Draw("colz");

	f->Write();

}