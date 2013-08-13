// with GMStau add a strange point to the model independant plot at 557 coming from Preselec of standard analysis, why?! 
void Compare_ModIndvsStandard() {
	TTree *T = new TTree("ntuple","data from ascii file");
	char Signal[20]; char filename[30];
	sprintf(Signal,"PPStau");
	//sprintf(Signal,"GMStau");
	sprintf(filename,"all_%s.csv",Signal);

	Long64_t nlines = T->ReadFile(filename,"Ms:type:Mcut:EffT:ErrEffT:EffPS:ErrEffPS:Eff:ErrEff");
	printf(" found %lld points\n",nlines);

	for (int i=0;i<6;i++){
		int Mt=100*i;
		char cond1[20];
		char cond2[20];
		sprintf(cond1,"type==1 & Mcut==%d",Mt);
		sprintf(cond2,"type==2 & Mcut==%d",Mt);
		ntuple->Draw("Ms:Eff:ErrEff",cond1);
		TGraphErrors *g1 =new TGraphErrors(ntuple->GetEntries(),ntuple->GetV1(),ntuple->GetV2(),0,ntuple->GetV3());  
		g1->SetMarkerColor(kRed);
		g1->SetMarkerStyle(7);

		ntuple->Draw("Ms:Eff:ErrEff",cond2);
		TGraphErrors *g2 =new TGraphErrors(ntuple->GetEntries(),ntuple->GetV1(),ntuple->GetV2(),0,ntuple->GetV3());  
		g2->SetMarkerColor(kBlue);
		g2->SetMarkerStyle(7);

		TCanvas *c2 = new TCanvas("c1","Efficiencies graphs",200,10,800,600);
		c2->cd();

		leg = new TLegend(0.1,0.8,0.28,0.9);
		leg->AddEntry(g1,"Model Indep.","p");
		leg->AddEntry(g2,"Standard An.","p");


		TMultiGraph *mg = new TMultiGraph();
		char title[100],SaveName[200];
	       	sprintf(title,"Efficiencies comparison Mod. Ind vs standard MCut=%d",i*100);	
		sprintf(SaveName,"pictures/Comparison_ModIndvsStand_%s_MCut%d.png",Signal,i*100);	
		mg->SetTitle(title);
		mg->Add(g1);
		mg->Add(g2);
		mg->Draw("AP");
		mg->GetXaxis()->SetTitle("Signal sample Mass");
		mg->GetYaxis()->SetTitle("Efficiency");
		leg->Draw();
		c2->SaveAs(SaveName);
	}


		int Mt=0;
		char cond1[20];
		char cond2[20];
		sprintf(cond1,"type==1 & Mcut==%d",Mt);
		sprintf(cond2,"type==2 & Mcut==%d",Mt);
		ntuple->Draw("Ms:EffT:ErrEffT",cond1);
		TGraphErrors *g3 =new TGraphErrors(ntuple->GetEntries(),ntuple->GetV1(),ntuple->GetV2(),0,ntuple->GetV3());  
		g3->SetMarkerColor(kRed);
		g3->SetMarkerStyle(7);

		ntuple->Draw("Ms:EffT:ErrEffT",cond2);
		TGraphErrors *g4 =new TGraphErrors(ntuple->GetEntries(),ntuple->GetV1(),ntuple->GetV2(),0,ntuple->GetV3());  
		g4->SetMarkerColor(kBlue);
		g4->SetMarkerStyle(7);

		TCanvas *c3 = new TCanvas("c3","Efficiencies graphs",200,10,800,600);
		c3->cd();

		leg3 = new TLegend(0.1,0.8,0.28,0.9);
		leg3->AddEntry(g3,"Model Indep.","p");
		leg3->AddEntry(g4,"Standard An.","p");


		TMultiGraph *mg3 = new TMultiGraph();
		char title[100],SaveName[200];
	       	sprintf(title,"Trigg. Efficiencies comparison Mod. Ind vs standard");	
		sprintf(SaveName,"pictures/Comparison_ModIndvsStand_%s_Trigg.png",Signal);	
		mg3->SetTitle(title);
		mg3->Add(g3);
		mg3->Add(g4);
		mg3->Draw("AP");
		mg3->GetXaxis()->SetTitle("Signal sample Mass");
		mg3->GetYaxis()->SetTitle("Efficiency");
		leg3->Draw();
		c3->SaveAs(SaveName);



		ntuple->Draw("Ms:EffPS:ErrEffPS",cond1);
		TGraphErrors *g5 =new TGraphErrors(ntuple->GetEntries(),ntuple->GetV1(),ntuple->GetV2(),0,ntuple->GetV3());  
		g5->SetMarkerColor(kRed);
		g5->SetMarkerStyle(7);

		ntuple->Draw("Ms:EffPS:ErrEffPS",cond2);
		TGraphErrors *g6 =new TGraphErrors(ntuple->GetEntries(),ntuple->GetV1(),ntuple->GetV2(),0,ntuple->GetV3());  
		g6->SetMarkerColor(kBlue);
		g6->SetMarkerStyle(7);


		ntuple->Draw("Ms:Eff:ErrEff",cond1);
		TGraphErrors *g7 =new TGraphErrors(ntuple->GetEntries(),ntuple->GetV1(),ntuple->GetV2(),0,ntuple->GetV3());  
		g7->SetMarkerColor(kRed);
		g7->SetMarkerStyle(7);

		ntuple->Draw("Ms:Eff:ErrEff",cond2);
		TGraphErrors *g8 =new TGraphErrors(ntuple->GetEntries(),ntuple->GetV1(),ntuple->GetV2(),0,ntuple->GetV3());  
		g8->SetMarkerColor(kBlue);
		g8->SetMarkerStyle(7);

		TCanvas *c4 = new TCanvas("c4","Efficiencies graphs",200,10,800,600);
		c4->cd();

		leg4 = new TLegend(0.1,0.8,0.28,0.9);
		leg4->AddEntry(g5,"Model Indep.","p");
		leg4->AddEntry(g6,"Standard An.","p");


		TMultiGraph *mg4 = new TMultiGraph();
	       	sprintf(title,"PreSel. Efficiencies comparison Mod. Ind vs standard");	
		sprintf(SaveName,"pictures/Comparison_ModIndvsStand_%s_PreSel.png",Signal);	
		mg4->SetTitle(title);
		mg4->Add(g5);
		mg4->Add(g6);
		mg4->Draw("AP");
		mg4->GetXaxis()->SetTitle("Signal sample Mass");
		mg4->GetYaxis()->SetTitle("Efficiency");
		leg4->Draw();
		c4->SaveAs(SaveName);

		TCanvas *c5 = new TCanvas("c5","Efficiencies graphs",200,10,800,600);
		c5->cd();

		leg5 = new TLegend(0.1,0.8,0.28,0.9);
		leg5->AddEntry(g7,"Model Indep.","p");
		leg5->AddEntry(g8,"Standard An.","p");


		TMultiGraph *mg5 = new TMultiGraph();
	       	sprintf(title,"Sel. Efficiencies comparison Mod. Ind vs standard");	
		sprintf(SaveName,"pictures/Comparison_ModIndvsStand_%s_Sel.png",Signal);	
		mg5->SetTitle(title);
		mg5->Add(g7);
		mg5->Add(g8);
		mg5->Draw("AP");
		mg5->GetXaxis()->SetTitle("Signal sample Mass");
		mg5->GetYaxis()->SetTitle("Efficiency");
		leg5->Draw();
		c5->SaveAs(SaveName);


}
