#pragma once

namespace ObjCategorizerForms {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			// Create three items and three sets of subitems for each item.
			ListViewItem^ item1 = gcnew ListViewItem( "item1",0 );

			// Place a check mark next to the item.
			item1->Checked = true;
			item1->SubItems->Add( "1" );
			item1->SubItems->Add( "2" );
			item1->SubItems->Add( "3" );
			array<ListViewItem^>^temp1 = {item1};
			resListView->Items->AddRange( temp1);
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  openBtn;
	protected: 

	protected: 

	private: System::Windows::Forms::TextBox^  imgPathTextBox;
	private: System::Windows::Forms::PictureBox^  openedPicBox;
	private: System::Windows::Forms::GroupBox^  trainGroupBox;

	private: System::Windows::Forms::GroupBox^  testGroupBox;

	private: System::Windows::Forms::Button^  predictBtn;
	private: System::Windows::Forms::GroupBox^  resultsGroupBox;
	private: System::Windows::Forms::ListView^  resListView;




	protected: 



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->openBtn = (gcnew System::Windows::Forms::Button());
			this->imgPathTextBox = (gcnew System::Windows::Forms::TextBox());
			this->openedPicBox = (gcnew System::Windows::Forms::PictureBox());
			this->trainGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->testGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->predictBtn = (gcnew System::Windows::Forms::Button());
			this->resultsGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->resListView = (gcnew System::Windows::Forms::ListView());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->openedPicBox))->BeginInit();
			this->testGroupBox->SuspendLayout();
			this->resultsGroupBox->SuspendLayout();
			this->SuspendLayout();
			// 
			// openBtn
			// 
			this->openBtn->Font = (gcnew System::Drawing::Font(L"Courier New", 7.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->openBtn->Location = System::Drawing::Point(347, 30);
			this->openBtn->Name = L"openBtn";
			this->openBtn->Size = System::Drawing::Size(84, 34);
			this->openBtn->TabIndex = 0;
			this->openBtn->Text = L"Open";
			this->openBtn->UseVisualStyleBackColor = true;
			this->openBtn->Click += gcnew System::EventHandler(this, &Form1::uploadBtn_Click);
			// 
			// imgPathTextBox
			// 
			this->imgPathTextBox->Location = System::Drawing::Point(38, 35);
			this->imgPathTextBox->Name = L"imgPathTextBox";
			this->imgPathTextBox->Size = System::Drawing::Size(266, 22);
			this->imgPathTextBox->TabIndex = 1;
			this->imgPathTextBox->Text = L"Show Upload Image Path";
			// 
			// openedPicBox
			// 
			this->openedPicBox->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->openedPicBox->Location = System::Drawing::Point(38, 92);
			this->openedPicBox->Name = L"openedPicBox";
			this->openedPicBox->Size = System::Drawing::Size(329, 262);
			this->openedPicBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->openedPicBox->TabIndex = 2;
			this->openedPicBox->TabStop = false;
			// 
			// trainGroupBox
			// 
			this->trainGroupBox->Location = System::Drawing::Point(28, 12);
			this->trainGroupBox->Name = L"trainGroupBox";
			this->trainGroupBox->Size = System::Drawing::Size(725, 150);
			this->trainGroupBox->TabIndex = 3;
			this->trainGroupBox->TabStop = false;
			this->trainGroupBox->Text = L"Training";
			// 
			// testGroupBox
			// 
			this->testGroupBox->Controls->Add(this->resultsGroupBox);
			this->testGroupBox->Controls->Add(this->predictBtn);
			this->testGroupBox->Controls->Add(this->openBtn);
			this->testGroupBox->Controls->Add(this->openedPicBox);
			this->testGroupBox->Controls->Add(this->imgPathTextBox);
			this->testGroupBox->Location = System::Drawing::Point(28, 181);
			this->testGroupBox->Name = L"testGroupBox";
			this->testGroupBox->Size = System::Drawing::Size(725, 388);
			this->testGroupBox->TabIndex = 4;
			this->testGroupBox->TabStop = false;
			this->testGroupBox->Text = L"Testing";
			// 
			// predictBtn
			// 
			this->predictBtn->Location = System::Drawing::Point(499, 92);
			this->predictBtn->Name = L"predictBtn";
			this->predictBtn->Size = System::Drawing::Size(86, 32);
			this->predictBtn->TabIndex = 4;
			this->predictBtn->Text = L"Predict";
			this->predictBtn->UseVisualStyleBackColor = true;
			// 
			// resultsGroupBox
			// 
			this->resultsGroupBox->Controls->Add(this->resListView);
			this->resultsGroupBox->Location = System::Drawing::Point(439, 147);
			this->resultsGroupBox->Name = L"resultsGroupBox";
			this->resultsGroupBox->Size = System::Drawing::Size(225, 207);
			this->resultsGroupBox->TabIndex = 5;
			this->resultsGroupBox->TabStop = false;
			this->resultsGroupBox->Text = L"Results";
			// 
			// resListView
			// 
			this->resListView->Location = System::Drawing::Point(40, 30);
			this->resListView->Name = L"resListView";
			this->resListView->Size = System::Drawing::Size(141, 160);
			this->resListView->TabIndex = 0;
			this->resListView->UseCompatibleStateImageBehavior = false;
			this->resListView->View = System::Windows::Forms::View::List;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(788, 590);
			this->Controls->Add(this->testGroupBox);
			this->Controls->Add(this->trainGroupBox);
			this->MaximizeBox = false;
			this->Name = L"Form1";
			this->Text = L"Categorizer";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->openedPicBox))->EndInit();
			this->testGroupBox->ResumeLayout(false);
			this->testGroupBox->PerformLayout();
			this->resultsGroupBox->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void uploadBtn_Click(System::Object^  sender, System::EventArgs^  e) {

				 // open image file
				 OpenFileDialog^ openImgDialog = gcnew OpenFileDialog;

				 openImgDialog->InitialDirectory = "c:\\";
				 openImgDialog->Filter = "Image Files(*.BMP;*.JPG;*.GIF)|*.BMP;*.JPG;*.GIF";
				 openImgDialog->FilterIndex = 1;
				 openImgDialog->RestoreDirectory = false;

				 if( openImgDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK )
				 {
					 // image path
					 String^ imgpath = openImgDialog->FileName;
					 imgPathTextBox->Text = imgpath;
					 // load in box
					 openedPicBox->Load(imgpath);

				 }
				 
			 }
	};
}

