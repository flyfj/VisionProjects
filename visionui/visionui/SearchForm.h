#pragma once


#pragma managed(push, off)
#include <opencv2/opencv.hpp>
#pragma managed(pop)

namespace visionui {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class SearchForm : public System::Windows::Forms::Form
	{
	public:
		SearchForm(void)
		{
			InitializeComponent();
			
			resImgBoxes.Add(resImgBox1);
			resImgBoxes.Add(resImgBox2);
			resImgBoxes.Add(resImgBox3);
			resImgBoxes.Add(resImgBox4);
			resImgBoxes.Add(resImgBox5);
			resImgBoxes.Add(resImgBox6);

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~SearchForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::PictureBox^  queryImgBox;
	private: System::Windows::Forms::TextBox^  queryImgPathTextBox;
	private: System::Windows::Forms::Button^  loadImgBtn;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::PictureBox^  resImgBox6;
	private: System::Windows::Forms::PictureBox^  resImgBox5;
	private: System::Windows::Forms::PictureBox^  resImgBox3;
	private: System::Windows::Forms::PictureBox^  resImgBox4;
	private: System::Windows::Forms::PictureBox^  resImgBox2;
	private: System::Windows::Forms::PictureBox^  resImgBox1;
	private: System::Windows::Forms::Button^  searchBtn;
	private: ArrayList resImgBoxes;
	

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
			this->queryImgBox = (gcnew System::Windows::Forms::PictureBox());
			this->queryImgPathTextBox = (gcnew System::Windows::Forms::TextBox());
			this->loadImgBtn = (gcnew System::Windows::Forms::Button());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->resImgBox6 = (gcnew System::Windows::Forms::PictureBox());
			this->resImgBox5 = (gcnew System::Windows::Forms::PictureBox());
			this->resImgBox3 = (gcnew System::Windows::Forms::PictureBox());
			this->resImgBox4 = (gcnew System::Windows::Forms::PictureBox());
			this->resImgBox2 = (gcnew System::Windows::Forms::PictureBox());
			this->resImgBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->searchBtn = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->queryImgBox))->BeginInit();
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->resImgBox6))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->resImgBox5))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->resImgBox3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->resImgBox4))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->resImgBox2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->resImgBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// queryImgBox
			// 
			this->queryImgBox->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->queryImgBox->Location = System::Drawing::Point(12, 12);
			this->queryImgBox->Name = L"queryImgBox";
			this->queryImgBox->Size = System::Drawing::Size(330, 293);
			this->queryImgBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->queryImgBox->TabIndex = 0;
			this->queryImgBox->TabStop = false;
			// 
			// queryImgPathTextBox
			// 
			this->queryImgPathTextBox->Location = System::Drawing::Point(12, 326);
			this->queryImgPathTextBox->Name = L"queryImgPathTextBox";
			this->queryImgPathTextBox->Size = System::Drawing::Size(330, 20);
			this->queryImgPathTextBox->TabIndex = 4;
			// 
			// loadImgBtn
			// 
			this->loadImgBtn->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->loadImgBtn->Location = System::Drawing::Point(55, 362);
			this->loadImgBtn->Name = L"loadImgBtn";
			this->loadImgBtn->Size = System::Drawing::Size(103, 29);
			this->loadImgBtn->TabIndex = 3;
			this->loadImgBtn->Text = L"Select Image";
			this->loadImgBtn->UseVisualStyleBackColor = true;
			this->loadImgBtn->Click += gcnew System::EventHandler(this, &SearchForm::loadImgBtn_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->resImgBox6);
			this->groupBox1->Controls->Add(this->resImgBox5);
			this->groupBox1->Controls->Add(this->resImgBox3);
			this->groupBox1->Controls->Add(this->resImgBox4);
			this->groupBox1->Controls->Add(this->resImgBox2);
			this->groupBox1->Controls->Add(this->resImgBox1);
			this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox1->Location = System::Drawing::Point(370, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(629, 409);
			this->groupBox1->TabIndex = 5;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Results";
			// 
			// resImgBox6
			// 
			this->resImgBox6->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->resImgBox6->Location = System::Drawing::Point(429, 216);
			this->resImgBox6->Name = L"resImgBox6";
			this->resImgBox6->Size = System::Drawing::Size(171, 163);
			this->resImgBox6->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->resImgBox6->TabIndex = 0;
			this->resImgBox6->TabStop = false;
			// 
			// resImgBox5
			// 
			this->resImgBox5->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->resImgBox5->Location = System::Drawing::Point(226, 216);
			this->resImgBox5->Name = L"resImgBox5";
			this->resImgBox5->Size = System::Drawing::Size(171, 163);
			this->resImgBox5->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->resImgBox5->TabIndex = 0;
			this->resImgBox5->TabStop = false;
			// 
			// resImgBox3
			// 
			this->resImgBox3->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->resImgBox3->Location = System::Drawing::Point(429, 25);
			this->resImgBox3->Name = L"resImgBox3";
			this->resImgBox3->Size = System::Drawing::Size(171, 163);
			this->resImgBox3->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->resImgBox3->TabIndex = 0;
			this->resImgBox3->TabStop = false;
			// 
			// resImgBox4
			// 
			this->resImgBox4->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->resImgBox4->Location = System::Drawing::Point(22, 216);
			this->resImgBox4->Name = L"resImgBox4";
			this->resImgBox4->Size = System::Drawing::Size(171, 163);
			this->resImgBox4->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->resImgBox4->TabIndex = 0;
			this->resImgBox4->TabStop = false;
			// 
			// resImgBox2
			// 
			this->resImgBox2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->resImgBox2->Location = System::Drawing::Point(226, 25);
			this->resImgBox2->Name = L"resImgBox2";
			this->resImgBox2->Size = System::Drawing::Size(171, 163);
			this->resImgBox2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->resImgBox2->TabIndex = 0;
			this->resImgBox2->TabStop = false;
			// 
			// resImgBox1
			// 
			this->resImgBox1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->resImgBox1->Location = System::Drawing::Point(22, 25);
			this->resImgBox1->Name = L"resImgBox1";
			this->resImgBox1->Size = System::Drawing::Size(171, 163);
			this->resImgBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->resImgBox1->TabIndex = 0;
			this->resImgBox1->TabStop = false;
			// 
			// searchBtn
			// 
			this->searchBtn->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->searchBtn->Location = System::Drawing::Point(183, 362);
			this->searchBtn->Name = L"searchBtn";
			this->searchBtn->Size = System::Drawing::Size(103, 29);
			this->searchBtn->TabIndex = 3;
			this->searchBtn->Text = L"Search";
			this->searchBtn->UseVisualStyleBackColor = true;
			this->searchBtn->Click += gcnew System::EventHandler(this, &SearchForm::loadImgBtn_Click);
			// 
			// SearchForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1021, 446);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->queryImgPathTextBox);
			this->Controls->Add(this->searchBtn);
			this->Controls->Add(this->loadImgBtn);
			this->Controls->Add(this->queryImgBox);
			this->Name = L"SearchForm";
			this->Text = L"VisualSearch";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->queryImgBox))->EndInit();
			this->groupBox1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->resImgBox6))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->resImgBox5))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->resImgBox3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->resImgBox4))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->resImgBox2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->resImgBox1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void loadImgBtn_Click(System::Object^  sender, System::EventArgs^  e) {

				 OpenFileDialog^ openImgDialog = gcnew OpenFileDialog();
				 openImgDialog->InitialDirectory = "D:\\";
				 openImgDialog->Filter = "Image Files(*.BMP;*.JPG;*.PNG;*.GIF)|*.BMP;*.JPG;*.GIF;*.PNG";
				 openImgDialog->FilterIndex = 1;
				 openImgDialog->RestoreDirectory = true;

				 if( openImgDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK )
				 {
					 // image path
					 System::String^ imgpath = openImgDialog->FileName;
					 queryImgPathTextBox->Text = openImgDialog->FileName;
					 // load in box
					 queryImgBox->Load(imgpath);
				 }

			 }
	};
}

