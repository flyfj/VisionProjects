//////////////////////////////////////////////////////////////////////////

#include "MainEngine.h"


namespace visualsearch
{

	MainEngine::MainEngine(void)
	{
		// set default parameters; can also be set outside
		dataset_dir = "H:\\AmazonProductData\\"; //"F:\\Datasets\\MobileProductSearch\\Whole_Dataset\\AmazonProductSet\\";
		img_dir = dataset_dir + "DemoDataSet_resized\\";
		product_category_list_file = dataset_dir + "clotheslist.txt";
		training_objects_info_file = dataset_dir + "uobjects.txt";
		salientObj_dir = dataset_dir + "amazon_salient\\";
		surfData_dir = dataset_dir + "DemoDataSet_surf\\";
		// results
		results_dir = "H:\\MobileVisualSearch\\";	//"F:\\Results\\MobileVisualSearch\\";	
		res_desc_dir = results_dir + "descs\\";
		res_bow_dir = results_dir + "bow_descriptors\\";
		// model
		res_dict_dir = results_dir + "dicts\\";
		global_dict_file = results_dir + "newsurf_d1000.dict";
		dict_size = 1000;
		res_model_dir = results_dir + "models\\";
		global_searcher_file = res_model_dir + "global.searcher";

		// set sign
		m_bUseSURF = true;
		task_TrainingDataInfo = LOAD;
		m_bUseSalientObjects = false;
		task_KeyDescriptors = LOAD;
		task_BOW = LOAD;
		task_Dicts = LOAD;
		m_bUseUniversalDict = true;
		m_bUseUniversalSearcher = true;
		task_Searcher = SAVE;

	}

	
	void MainEngine::PrepareCategoryDatabaseForTraining()
	{
		// load training category objects information
		int trainingSampleNum = 200;
		string cate_name;
		int count = -1;

		srand ( time(NULL) );
		ifstream in(product_category_list_file.c_str());

		if(task_TrainingDataInfo == SAVE)
		{
			// randomly select images for each category as training samples
			while(in>>cate_name)
			{
				count++;

				// get files in category dir
				string dirname = img_dir + cate_name + "\\";
				FileInfos fileInfos;
				GetFilesFromDir(dirname, "*.jpg", fileInfos);

				// generate random index sequence
				IntVector file_ids;
				for(size_t i=0; i<fileInfos.size(); i++)
					file_ids.push_back(i);
				for(size_t i=0; i<fileInfos.size(); i++)
				{
					swap(file_ids[i], file_ids[rand()%file_ids.size()]);
				}

				// create products
				ImageObjectCategory obj_cate;
				obj_cate.category_name = cate_name;
				ImageObjects imgObjs;
				imgObjs.imgObj.categoryName = cate_name;
				imgObjs.imgObj.category_id = count;

				// randomly pick N images as training samples
				int total_sz = MIN(trainingSampleNum, fileInfos.size());
				for(int i=0; i<total_sz; i++)
				{
					int id = file_ids[i];
					imgObjs.imgObj.imgname = fileInfos[id].filename;
					// only save path from category name
					int start_pos = fileInfos[id].filepath.find(cate_name);
					int len = fileInfos[id].filepath.length();
					imgObjs.imgObj.imgpath = fileInfos[id].filepath.substr(start_pos, len-start_pos);
					imgObjs.imgObj.imgfile = img_dir + imgObjs.imgObj.imgpath;

					// load object information
					if(m_bUseSalientObjects)
					{
						int objNumPerImage = 20;
						ImageObjects& curImageObjects = imgObjs;
						string curpath = curImageObjects.imgObj.imgpath;
						string salientobjfile = salientObj_dir + curpath.substr(0, curpath.length()-4) + "_0.60_res.txt";
						ifstream objin(salientobjfile.c_str());
						if(objin.fail())
							continue;

						// read object from file and compute features
						int boxx;
						float score;
						for(int k=0; k<objNumPerImage; k++)
						{
							if(objin>>boxx)
							{
								// set object general info
								VisualObject tempobj;
								tempobj.categoryName = curImageObjects.imgObj.categoryName;
								tempobj.category_id = curImageObjects.imgObj.category_id;
								tempobj.imgfile = curImageObjects.imgObj.imgfile;
								tempobj.imgpath = curImageObjects.imgObj.imgpath;
								// set box
								tempobj.visual_desc.bbox.x = boxx;
								objin>>tempobj.visual_desc.bbox.y>>tempobj.visual_desc.bbox.width>>tempobj.visual_desc.bbox.height;
								objin>>score;

								// add to objects collection
								curImageObjects.objects.push_back( tempobj );

							}
							else	// not enough objects
								break;
						}
					}
					
					obj_cate.imgObjs.push_back( imgObjs );

				}

				training_objects.push_back( obj_cate );

				cout<<"Finish picking "<<trainingSampleNum<<" training objects from product "<<cate_name<<endl;
			}

			// save all objects used
			ofstream out(training_objects_info_file.c_str());
			for(size_t i=0; i<training_objects.size(); i++)
			{
				out<<training_objects[i].category_name<<" "<<training_objects[i].imgObjs.size()<<endl;
				for(size_t j=0; j<training_objects[i].imgObjs.size(); j++)
					out<<training_objects[i].imgObjs[j].imgObj.imgpath<<endl;
			}

			cout<<"Save training products info to "<<product_category_list_file<<endl;
		}
		else
		{
			ifstream in1(training_objects_info_file.c_str());
			// load info from file
			int obj_num;
			int count = -1;
			string cate_name;
			// each category
			while(in1>>cate_name>>obj_num)
			{
				count++;
				ImageObjectCategory obj_cate;
				obj_cate.category_name = cate_name;
				obj_cate.imgObjs.resize(obj_num);
				// each image
				for(int i=0; i<obj_num; i++)
				{
					obj_cate.imgObjs[i].imgObj.categoryName = cate_name;
					obj_cate.imgObjs[i].imgObj.category_id = count;
					in1>>obj_cate.imgObjs[i].imgObj.imgpath;
					obj_cate.imgObjs[i].imgObj.imgfile = img_dir + obj_cate.imgObjs[i].imgObj.imgpath;

					// load object information
					if(m_bUseSalientObjects)
					{
						int objNumPerImage = 20;
						ImageObjects& curImageObjects = obj_cate.imgObjs[i];
						string curpath = curImageObjects.imgObj.imgpath;
						string salientobjfile = salientObj_dir + curpath.substr(0, curpath.length()-4) + "_0.60_res.txt";
						ifstream objin(salientobjfile.c_str());
						if(objin.fail())
							continue;

						// read object from file and compute features
						int boxx;
						float score;
						for(int k=0; k<objNumPerImage; k++)
						{
							if(objin>>boxx)
							{
								// set object general info
								VisualObject tempobj;
								tempobj.categoryName = curImageObjects.imgObj.categoryName;
								tempobj.category_id = curImageObjects.imgObj.category_id;
								tempobj.imgfile = curImageObjects.imgObj.imgfile;
								tempobj.imgpath = curImageObjects.imgObj.imgpath;
								// set box
								tempobj.visual_desc.bbox.x = boxx;
								objin>>tempobj.visual_desc.bbox.y>>tempobj.visual_desc.bbox.width>>tempobj.visual_desc.bbox.height;
								objin>>score;

								// add to objects collection
								curImageObjects.objects.push_back( tempobj );

							}
							else	// not enough objects
								break;
						}
					}

					trainingFiles.push_back(obj_cate.imgObjs[i].imgObj.imgfile);
				}

				training_objects.push_back(obj_cate);

				cout<<"Load product "<<cate_name<<endl;
			}
		}
		
	}



	bool MainEngine::Train()
	{

		if(training_objects.empty())
		{
			cerr<<"No training data for MainEngine."<<endl;
			return false;
		}



		if(task_Searcher == SAVE)
		{

			/************************************************************************/
			/* extract image keypoints and local descriptors                        */
			/************************************************************************/
			mkdir(results_dir.c_str());
			mkdir(res_desc_dir.c_str());

			for(size_t i=0; i<training_objects.size(); i++)
			{
				// each category objects
				ImageObjectCategory& cur_category = training_objects[i];

				string desc_res_dir = res_desc_dir + cur_category.category_name + "\\";
				mkdir(desc_res_dir.c_str());

				// object data
				cv_Images training_imgs(cur_category.imgObjs.size());
				vector<cv_KeyPoints> imgs_keypoints(cur_category.imgObjs.size());
				vector<Mat> imgs_descriptors(cur_category.imgObjs.size());
				StringVector desc_data_files(cur_category.imgObjs.size());

				// load image
				for(size_t j=0; j<cur_category.imgObjs.size(); j++)
				{
					training_imgs[j] = imread(img_dir + cur_category.imgObjs[j].imgObj.imgpath);
					if(training_imgs[j].empty())
					{
						cerr<<"Fail to read training image file."<<endl;
						return -1;
					}

					// set desc data file
					if(m_bUseSURF)
						desc_data_files[j] = surfData_dir + cur_category.imgObjs[j].imgObj.imgpath + ".surf";
					else
						desc_data_files[j] = res_desc_dir + cur_category.imgObjs[j].imgObj.imgpath + ".desc";
				}

				if(m_bUseSURF)
				{
					// load surf data
					for(size_t j=0; j<desc_data_files.size(); j++)
					{

						desc_extractor.LoadSurfDescriptor(desc_data_files[j], imgs_keypoints[j], imgs_descriptors[j]);

						// set data
						cur_category.imgObjs[j].imgObj.visual_desc.pts = imgs_keypoints[j];
						cur_category.imgObjs[j].imgObj.visual_desc.descs = imgs_descriptors[j].clone();
					}
				}
				else
				{
					// detect keypoints
					keypoint_detector.ExtractImgKeypoints(training_imgs, imgs_keypoints, "FAST");

					// extract descriptors
					desc_extractor.ExtractImgKeypointDescriptors(training_imgs, imgs_keypoints, imgs_descriptors, "SURF");

					// save descriptors data
					for(size_t j=0; j<desc_data_files.size(); j++)
					{
						if(imgs_keypoints[j].empty())
							continue;

						// save descriptors
						desc_extractor.SaveDescriptors(desc_data_files[j], imgs_keypoints[j], imgs_descriptors[j]);

						// save object information
						cur_category.imgObjs[j].imgObj.visual_desc.pts = imgs_keypoints[j];
						cur_category.imgObjs[j].imgObj.visual_desc.descs = imgs_descriptors[j].clone();
					}

				}

			}

		}




		/************************************************************************/
		/* create dictionary                                                    */
		/************************************************************************/
		mkdir(res_dict_dir.c_str());

		if(task_Dicts == LOAD)
		{
			if(m_bUseUniversalDict)
			{
				dict_generators.resize(1);
				// load universal dict
				dict_generators[0].LoadDictionary(global_dict_file);
			}
			else
			{
				dict_generators.resize(training_objects.size());
				for(size_t i=0; i<training_objects.size(); i++)
				{
					string cate_dict_file = res_dict_dir + training_objects[i].category_name + ".dict";
					dict_generators[i].LoadDictionary(cate_dict_file);
				}
			}
		}
		else
		{
			// build
			if(m_bUseUniversalDict)
			{
				dict_generators.resize(1);

				// collect all descriptors data
				Mat all_descriptors;
				for(size_t i=0; i<training_objects.size(); i++)
				{
					for(size_t j=0; j<training_objects[i].imgObjs.size(); j++)
					{
						all_descriptors.push_back( training_objects[i].imgObjs[j].imgObj.visual_desc.descs );
					}
				}

				// learn
				dict_generators[0].BuildDictionary(all_descriptors, dict_size, "KMEANS");
				// save
				dict_generators[0].SaveDictionary(global_dict_file);
			}
			else
			{
				dict_generators.resize(training_objects.size());

				vector<Mat> all_descriptors(training_objects.size());
				for(size_t i=0; i<training_objects.size(); i++)
				{
					for(size_t j=0; j<training_objects[i].imgObjs.size(); j++)
					{
						all_descriptors[i].push_back( training_objects[i].imgObjs[j].imgObj.visual_desc.descs );
					}

					// learn
					dict_generators[i].BuildDictionary(all_descriptors[i], dict_size, "KMEANS");
					// save
					string dict_path = res_dict_dir + training_objects[i].category_name + ".dict";
					dict_generators[i].SaveDictionary(dict_path);
				}
			}

		}

		

		if(task_Searcher == SAVE)
		{

			// only needed when train the searchers
			/************************************************************************/
			/* extract object bow features                                          */
			/************************************************************************/
			mkdir(res_bow_dir.c_str());
			for(size_t i=0; i<training_objects.size(); i++)
			{
				string cate_bow_dir = res_bow_dir + training_objects[i].category_name + "\\";
				mkdir(cate_bow_dir.c_str());

				ImageObjectCategory& cur_category = training_objects[i];

				if(task_BOW == LOAD)
				{
					if(m_bUseSalientObjects)
					{
						for(size_t j=0; j<cur_category.imgObjs.size(); j++)
						{
							ImageObjects& curImageObjects = cur_category.imgObjs[j];
							string curpath = curImageObjects.imgObj.imgpath;
							// load
							string res_salientobjfile = res_bow_dir + curImageObjects.imgObj.imgpath + ".objs";
							LoadImageObjects(res_salientobjfile, curImageObjects);
						}
					}
					else
					{
						for(size_t j=0; j<cur_category.imgObjs.size(); j++)
						{
							VisualObject& curimg = cur_category.imgObjs[j].imgObj;
							// load from data file
							string imgbowfile = res_bow_dir + curimg.imgpath + ".bow";
							img_descriptor_generator.LoadImgDescriptor(imgbowfile, curimg.visual_desc.img_desc);
						}
					}
				}
				else
				{
					if(m_bUseSalientObjects)
					{
						// compute salient objects' bow and ignore whole image feature
						// only support universal dictionary now
						for(size_t j=0; j<cur_category.imgObjs.size(); j++)
						{
							ImageObjects& curImageObjects = cur_category.imgObjs[j];
							if(curImageObjects.objects.empty())	// no objects
								continue;

							for(size_t k=0; k<curImageObjects.objects.size(); k++)
							{
								VisualObject& tempobj = curImageObjects.objects[k];

								// compute object's bow feature
								// extract keypoints and local descriptors
								const ObjVisualDescription& cur_img_vdesc = curImageObjects.imgObj.visual_desc;
								for(size_t id=0; id<cur_img_vdesc.pts.size(); id++)
								{
									if(tempobj.visual_desc.bbox.contains( cur_img_vdesc.pts[id].pt ))
									{
										// add keypoint
										tempobj.visual_desc.pts.push_back(cur_img_vdesc.pts[id]);
										// add descriptor
										tempobj.visual_desc.descs.push_back( cur_img_vdesc.descs.row(id) );
									}
								}

								// compute bow descriptor
								img_descriptor_generator.SetDictionary(dict_generators[0].dictionary);
								img_descriptor_generator.ComputeBOWFromKeypointDescs(
									tempobj.visual_desc.descs, tempobj.visual_desc.img_desc);
							}

							// save objects information for each image
							string res_salientobjfile = res_bow_dir + curImageObjects.imgObj.imgpath + ".objs";
							SaveImageObjects(res_salientobjfile, curImageObjects);
						}

					}
					else
					{
						if(m_bUseUniversalDict)
							img_descriptor_generator.SetDictionary(dict_generators[0].dictionary);
						else
							img_descriptor_generator.SetDictionary(dict_generators[i].dictionary);

						// compute image bow
						for(size_t j=0; j<cur_category.imgObjs.size(); j++)
						{
							VisualObject& curimg = cur_category.imgObjs[j].imgObj;
							img_descriptor_generator.ComputeBOWFromKeypointDescs(
								curimg.visual_desc.descs, curimg.visual_desc.img_desc);

							// save to data file
							string imgbowfile = res_bow_dir + curimg.imgpath + ".bow";
							img_descriptor_generator.SaveImgDescriptor(imgbowfile, curimg.visual_desc.img_desc);
						}
					}
				}

			}
		}




		/************************************************************************/
		/* build searchers                                                      */
		/************************************************************************/
		obj_searchers.clear();
		if(task_Searcher == SAVE)
		{
			cout<<"Start building searcher..."<<endl;

			mkdir(res_model_dir.c_str());

			if(m_bUseUniversalSearcher)
			{
				obj_searchers.resize(1);
				// collect all objects from all categories
				vector<VisualObject> all_objects;

				for(size_t i=0; i<training_objects.size(); i++)
				{
					for(size_t j=0; j<training_objects[i].imgObjs.size(); j++)
					{
						if(m_bUseSalientObjects)
						{
							for(size_t k=0; k<training_objects[i].imgObjs[j].objects.size(); k++)
							{
								if( !training_objects[i].imgObjs[j].objects[k].visual_desc.img_desc.empty() )
									all_objects.push_back( training_objects[i].imgObjs[j].objects[k] );
							}
						}
						else
						{
							if( !training_objects[i].imgObjs[j].imgObj.visual_desc.img_desc.empty() )
								all_objects.push_back( training_objects[i].imgObjs[j].imgObj );
						}
					}
				}

				// build
				obj_searchers[0].Build(all_objects);

				obj_searchers[0].SaveSearcher(global_searcher_file);

			}
			else
			{
				obj_searchers.resize(training_objects.size());

				for(size_t i=0; i<training_objects.size(); i++)
				{
					// collect all objects from each category
					vector<VisualObject> all_objects;

					for(size_t j=0; j<training_objects[i].imgObjs.size(); j++)
					{
						if(m_bUseSalientObjects)
						{
							for(size_t k=0; k<training_objects[i].imgObjs[j].objects.size(); k++)
							{
								if(!training_objects[i].imgObjs[j].objects[k].visual_desc.img_desc.empty())
									all_objects.push_back( training_objects[i].imgObjs[j].objects[k] );
							}
						}
						else
						{
							if(!training_objects[i].imgObjs[j].imgObj.visual_desc.img_desc.empty())
								all_objects.push_back( training_objects[i].imgObjs[j].imgObj );
						}
					}

					// build
					obj_searchers[i].Build(all_objects);

				}
			}
		}
		else
		{
			if(m_bUseUniversalSearcher)
			{
				obj_searchers.resize(1);

				obj_searchers[0].LoadSearcher(global_searcher_file);
			}
			else
			{

			}
		}
		

		return true;
	}


	vector<SearchResults> MainEngine::Search(const ImageObjects& query_image)
	{
		vector<SearchResults> res;

		if(obj_searchers.empty())
		{
			cerr<<"No searcher yet."<<endl;
			return res;
		}

		if(m_bUseSalientObjects)
		{
			// search for each object
			for(size_t i=0; i<query_image.objects.size(); i++)
			{
				if(query_image.objects[i].visual_desc.img_desc.empty())
					continue;

				if(m_bUseUniversalSearcher)
				{
					SearchResults cur_res = obj_searchers[0].Search(query_image.objects[i], -1);
					res.push_back(cur_res);
				}
			}
		}
		else
		{
			if(m_bUseUniversalSearcher)
			{
				if(query_image.imgObj.visual_desc.img_desc.empty())
					return res;

				SearchResults cur_res = obj_searchers[0].Search(query_image.imgObj, -1);
				res.push_back(cur_res);
			}
		}

		return res;
	}


	void MainEngine::TestSearch()
	{

		int querynum = 10;
		string cate_name;
		int count = -1;

		srand ( time(NULL) );
		ifstream in(product_category_list_file.c_str());

		// randomly select images for each category as query samples
		while(in>>cate_name)
		{
			count++;

			// get files in category dir
			string dirname = img_dir + cate_name + "\\";
			FileInfos fileInfos;
			GetFilesFromDir(dirname, "*.jpg", fileInfos);

			// generate random index sequence
			IntVector file_ids;
			for(size_t i=0; i<fileInfos.size(); i++)
				file_ids.push_back(i);
			for(size_t i=0; i<fileInfos.size(); i++)
			{
				swap(file_ids[i], file_ids[rand()%file_ids.size()]);
			}

			// create products
			ImageObjectCategory obj_cate;
			obj_cate.category_name = cate_name;
			ImageObjects imgObjs;
			imgObjs.imgObj.categoryName = cate_name;
			imgObjs.imgObj.category_id = count;

			// randomly pick N images as training samples
			int total_sz = MIN(querynum, fileInfos.size());
			for(int i=0; i<total_sz; i++)
			{
				int id = file_ids[i];
				imgObjs.imgObj.imgname = fileInfos[id].filename;
				// only save path from category name
				int start_pos = fileInfos[id].filepath.find(cate_name);
				int len = fileInfos[id].filepath.length();
				imgObjs.imgObj.imgpath = fileInfos[id].filepath.substr(start_pos, len-start_pos);
				imgObjs.imgObj.imgfile = img_dir + imgObjs.imgObj.imgpath;

				// load object information
				if(m_bUseSalientObjects)
				{
					int objNumPerImage = 20;
					ImageObjects& curImageObjects = imgObjs;
					string curpath = curImageObjects.imgObj.imgpath;
					string salientobjfile = salientObj_dir + curpath.substr(0, curpath.length()-4) + "_0.60_res.txt";
					ifstream objin(salientobjfile.c_str());
					if(objin.fail())
						continue;

					// read object from file and compute features
					int boxx;
					float score;
					for(int k=0; k<objNumPerImage; k++)
					{
						if(objin>>boxx)
						{
							// set object general info
							VisualObject tempobj;
							tempobj.categoryName = curImageObjects.imgObj.categoryName;
							tempobj.category_id = curImageObjects.imgObj.category_id;
							tempobj.imgfile = curImageObjects.imgObj.imgfile;
							tempobj.imgpath = curImageObjects.imgObj.imgpath;
							// set box
							tempobj.visual_desc.bbox.x = boxx;
							objin>>tempobj.visual_desc.bbox.y>>tempobj.visual_desc.bbox.width>>tempobj.visual_desc.bbox.height;
							objin>>score;

							// add to objects collection
							curImageObjects.objects.push_back( tempobj );

						}
						else	// not enough objects
							break;
					}
				}

				obj_cate.imgObjs.push_back(imgObjs);
			}

			query_objects.push_back( obj_cate );

			cout<<"Finish picking "<<querynum<<" query objects from product "<<cate_name<<endl;
		}

		// compute surf
		for(size_t i=0; i<query_objects.size(); i++)
		{
			// each category objects
			ImageObjectCategory& cur_category = query_objects[i];

			// object data
			StringVector desc_data_files(cur_category.imgObjs.size());
			vector<cv_KeyPoints> imgs_keypoints(cur_category.imgObjs.size());
			vector<Mat> imgs_descriptors(cur_category.imgObjs.size());

			// load image
			for(size_t j=0; j<cur_category.imgObjs.size(); j++)
			{
				// set desc data file
				if(m_bUseSURF)
					desc_data_files[j] = surfData_dir + cur_category.imgObjs[j].imgObj.imgpath + ".surf";
			}

			if(m_bUseSURF)
			{
				// load surf data
				for(size_t j=0; j<desc_data_files.size(); j++)
				{
					desc_extractor.LoadSurfDescriptor(desc_data_files[j], imgs_keypoints[j], imgs_descriptors[j]);

					// set data
					cur_category.imgObjs[j].imgObj.visual_desc.pts = imgs_keypoints[j];
					cur_category.imgObjs[j].imgObj.visual_desc.descs = imgs_descriptors[j].clone();
				}
			}
		}

		// compute bow
		for(size_t i=0; i<query_objects.size(); i++)
		{
			ImageObjectCategory& cur_category = query_objects[i];

			if(m_bUseSalientObjects)
			{

				// compute salient objects' bow and ignore whole image feature
				// only support universal dictionary now
				img_descriptor_generator.SetDictionary(dict_generators[0].dictionary);

				for(size_t j=0; j<cur_category.imgObjs.size(); j++)
				{
					ImageObjects& curImageObjects = cur_category.imgObjs[j];
					if(curImageObjects.objects.empty())	// no objects
						continue;

					for(size_t k=0; k<curImageObjects.objects.size(); k++)
					{
						VisualObject& tempobj = curImageObjects.objects[k];

						// compute object's bow feature
						// extract keypoints and local descriptors
						const ObjVisualDescription& cur_img_vdesc = curImageObjects.imgObj.visual_desc;
						for(size_t id=0; id<cur_img_vdesc.pts.size(); id++)
						{
							if(tempobj.visual_desc.bbox.contains( cur_img_vdesc.pts[id].pt ))
							{
								// add keypoint
								tempobj.visual_desc.pts.push_back(cur_img_vdesc.pts[id]);
								// add descriptor
								tempobj.visual_desc.descs.push_back( cur_img_vdesc.descs.row(id) );
							}
						}

						// compute bow descriptor
						img_descriptor_generator.ComputeBOWFromKeypointDescs(
							tempobj.visual_desc.descs, tempobj.visual_desc.img_desc);
					}
				}
				
			}
			else
			{
				// compute image bow
				if(m_bUseUniversalDict)
					img_descriptor_generator.SetDictionary(dict_generators[0].dictionary);
				else
					img_descriptor_generator.SetDictionary(dict_generators[i].dictionary);

				for(size_t j=0; j<cur_category.imgObjs.size(); j++)
				{
					VisualObject& curimg = cur_category.imgObjs[j].imgObj;
					img_descriptor_generator.ComputeBOWFromKeypointDescs(
						curimg.visual_desc.descs, curimg.visual_desc.img_desc);
				}
			}
		}

		// search
		cout<<trainingFiles.size()<<endl;
		// each category
		for(size_t i=0; i<query_objects.size(); i++)
		{
			// search objects for each image
			for(size_t j=0; j<query_objects[i].imgObjs.size(); j++)
			{
				// check if in training data
				if( find(trainingFiles.begin(), trainingFiles.end(), query_objects[i].imgObjs[j].imgObj.imgfile) == trainingFiles.end() )
					cout<<"New sample"<<endl;
				else
              		cout<<"Train sample"<<endl;

				// do searching
				vector<SearchResults> res = Search(query_objects[i].imgObjs[j]);
				if(res.empty())
					continue;

				char str[30];
				// show results
				if(res.size()==1)
				{
					Mat queryimg = imread(query_objects[i].imgObjs[j].imgObj.imgfile);
					//rectangle(queryimg, query_objects[i].imgObjs[j].objects[i].visual_desc.bbox, CV_RGB(255,0,0));
					imshow("query", queryimg);
					waitKey(10);

					// show results
					for(size_t k=0; k<res[0].res_objs.size(); k++)
					{
						Mat img = imread(res[0].res_objs[k].imgfile);
						//rectangle(img, res[0].res_objs[j].visual_desc.bbox, CV_RGB(255,0,0));
						sprintf(str, "res%d", k);
						imshow(string(str), img);
						waitKey(10);
					}
					waitKey(0);
				}
				else
				{
					// for each query object
					for(size_t k=0; k<query_objects[i].imgObjs[j].objects.size(); k++)
					{
						Mat queryimg = imread(query_objects[i].imgObjs[j].objects[k].imgfile);
						rectangle(queryimg, query_objects[i].imgObjs[j].objects[k].visual_desc.bbox, CV_RGB(255,0,0));
						imshow("query", queryimg);
						waitKey(10);

						// show results
						for(size_t id=0; id<res[k].res_objs.size(); id++)
						{
							Mat img = imread(res[k].res_objs[id].imgfile);
							rectangle(img, res[k].res_objs[id].visual_desc.bbox, CV_RGB(255,0,0));
							sprintf(str, "res%d", id);
							imshow(string(str), img);
							waitKey(10);
						}
						waitKey(0);
					}
				}
				

			}
		}

	}


	//////////////////////////////////////////////////////////////////////////

	bool MainEngine::SaveImageObjects(const string& file, const ImageObjects& imgObjs)
	{
		ofstream out(file.c_str());
		out<<imgObjs.objects.size()<<endl;
		for(size_t i=0; i<imgObjs.objects.size(); i++)
		{
			if(imgObjs.objects[i].visual_desc.img_desc.empty())
				continue;

			const ObjVisualDescription& visual_desc = imgObjs.objects[i].visual_desc;
			// bounding box
			out<<visual_desc.bbox.x<<" "<<visual_desc.bbox.y<<" "<<visual_desc.bbox.width<<" "<<visual_desc.bbox.height<<endl;
			// output image descriptor dimension
			out<<visual_desc.img_desc.cols<<endl;
			for(int j=0; j<visual_desc.img_desc.cols; j++)
				out<<(j==0? "":" ")<<visual_desc.img_desc.at<float>(0, j);
			out<<endl;
		}

		cout<<"Save image objects to "<<file<<endl;

		return true;
	}

	bool MainEngine::LoadImageObjects(const string& file, ImageObjects& imgObjs)
	{

		ifstream in(file.c_str());
		if(in.fail())
			return false;

		int num;
		in>>num;
		imgObjs.objects.resize(num);
		for(size_t i=0; i<imgObjs.objects.size(); i++)
		{
			imgObjs.objects[i].category_id = imgObjs.imgObj.category_id;
			imgObjs.objects[i].categoryName = imgObjs.imgObj.categoryName;
			imgObjs.objects[i].imgfile = imgObjs.imgObj.imgfile;
			imgObjs.objects[i].imgpath = imgObjs.imgObj.imgpath;
			imgObjs.objects[i].imgname = imgObjs.imgObj.imgname;

			ObjVisualDescription& visual_desc = imgObjs.objects[i].visual_desc;
			while(in>>visual_desc.bbox.x)	// in case it's empty
			{
				in>>visual_desc.bbox.y>>visual_desc.bbox.width>>visual_desc.bbox.height;
				int dim;
				in>>dim;
				visual_desc.img_desc.create(1, dim, CV_32FC1);
				for(int j=0; j<dim; j++)
					in>>visual_desc.img_desc.at<float>(0,j);
				
				break;
			}
		}

		cout<<"Load image objects from "<<file<<endl;

		return true;
	}

}

