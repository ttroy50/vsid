#include "ProtocolModelDb.h"
#include "ProtocolModel.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include <string>
#include <iostream>
#include <cstdio>
#include "AttributeMeterFactory.h"
#include "AttributeMeterRegistrar.h"

#define BOOST_TEST_MODULE ProtocolModelDbTest
#include <boost/test/unit_test.hpp>

using namespace Vsid;
using namespace std;

INIT_LOGGING


void removePrev(string testFile)
{

	std::ifstream tmp(testFile);
	if ( !tmp.good() )
	{
		tmp.close();
		return;
	}
	tmp.close();
	if(std::remove(testFile.c_str()) != 0)
		cout << "Unable to remove test file" << endl;
}

/**
 * Checks the files for the existence of the correct files after writing
 *
 * @param  testFile
 *
 * @return
 */
bool checkFiles(string testFile)
{
	string prevFile = testFile;
	prevFile += ".prev";

	ifstream newDb(testFile);
	ifstream oldDb(prevFile);
	if(!newDb.is_open() || !oldDb.is_open())
	{
		SLOG_ERROR(<< "Unable to open DB");
		return false;
	}

	bool equal = true;
	/* 
	Can't rely on the order of the nodes so can't just compare files.
	Also YAML::Node doesn't have a comparison operator.
	Easiest way to check equality would be to run test tool again.
	
	while ((!newDb.eof()) && (!oldDb.eof())) 
	{
		string line,line2;
		getline(newDb,line); 
		getline(oldDb,line2);
        if(line != line2)
        {
        	SLOG_ERROR(<< "Line not equal newDb [" << line << "] oldDb [" << line2 << "]");
        	equal = false;
           	break;
    	}
    }*/
    newDb.close();
    oldDb.close();
    return equal;
}

void copyFile(string src, string dst)
{
    ifstream source(src, ios::binary);
    ofstream dest(dst, ios::binary);
    dest << source.rdbuf();
    source.close();
    dest.close();
}

BOOST_AUTO_TEST_CASE( test_protocol_model )
{	
    init_attribute_meters();

    int argc = boost::unit_test::framework::master_test_suite().argc;
    char ** argv = boost::unit_test::framework::master_test_suite().argv;
    string testDbFile;
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] != '-')
        {
            testDbFile = argv[i];
            break;
        }
    }

    if( testDbFile.empty() )
	   testDbFile = "../../../../src/protocol_model/unit_test/test_db.yaml";
    
    string copyOfTestDbFile = testDbFile + ".totest";

    // Copy the DB to a testable version because when we write we don't want to overwrite checked
    // in version
    removePrev( copyOfTestDbFile);
    copyFile(testDbFile, copyOfTestDbFile);

    string testDbFilePrev = copyOfTestDbFile + ".prev";
    removePrev( testDbFilePrev);

    ProtocolModelDb testDb( copyOfTestDbFile, testDbFilePrev );

    // check we can read DB
    BOOST_REQUIRE( testDb.read() );

    // DB size
    BOOST_CHECK_EQUAL( testDb.size(), 2 );
    BOOST_CHECK_EQUAL( testDb.definingLimit(), 10 );
    BOOST_CHECK_EQUAL( testDb.cutoffLimit(), 10000 );

    // We can use the find method
    std::shared_ptr<ProtocolModel> http_video_find = testDb.find("HTTP-Video");
    BOOST_REQUIRE( http_video_find );
    BOOST_CHECK_EQUAL( http_video_find->name(), "HTTP-Video" );
   
    // Check assesor methods for the protoccol model
    std::shared_ptr<ProtocolModel> http_video = testDb.front();
    BOOST_REQUIRE(http_video);
    BOOST_CHECK_EQUAL( http_video->name(), "HTTP-Video" );
    BOOST_CHECK_EQUAL( http_video->name(), http_video_find->name() );
    BOOST_CHECK_EQUAL( http_video->flowCount(), 55 );
    BOOST_CHECK_EQUAL( http_video->enabled(), true );
    BOOST_CHECK_EQUAL( http_video->portHints().size(), 2 );
    BOOST_CHECK_EQUAL( http_video->portHints()[0], 80 );
    BOOST_CHECK_EQUAL( http_video->portHints()[1], 8080 );
    // check attribute meters
    BOOST_CHECK_EQUAL( http_video->size(), 2 );
    BOOST_CHECK_EQUAL( http_video->find("DirectionBytesCountMeter")->name(), "DirectionBytesCountMeter" );
    BOOST_CHECK_EQUAL( http_video->front()->name(), "DirectionBytesCountMeter" );
    std::shared_ptr<AttributeMeter> dbcm = http_video->at(0);
    BOOST_REQUIRE(dbcm);
    BOOST_CHECK_EQUAL( dbcm->name(), "DirectionBytesCountMeter" );
    BOOST_CHECK_EQUAL( dbcm->enabled(), true );
    BOOST_CHECK_EQUAL( dbcm->flowCount(), 55 );
    std::vector<double> expected_dbcm_fp {0.34,0.66};
    BOOST_CHECK_EQUAL_COLLECTIONS(dbcm->fingerprint().begin(), dbcm->fingerprint().end(), 
                              		expected_dbcm_fp.begin(), expected_dbcm_fp.end());

    std::shared_ptr<ProtocolModel> https_video = testDb.at(1);
    BOOST_REQUIRE(https_video);
    BOOST_CHECK_EQUAL( https_video->name(), "HTTPS-Video" );
    BOOST_CHECK_EQUAL( https_video->flowCount(), 99 );
    BOOST_CHECK_EQUAL( https_video->enabled(), false );
    BOOST_CHECK_EQUAL( https_video->portHints().size(), 0 );
    std::shared_ptr<AttributeMeter> fpbm = https_video->find("ByteFrequencyFirstOrigToDestPacket");
    BOOST_CHECK_EQUAL( fpbm->enabled(), false );
    BOOST_CHECK_EQUAL( fpbm->flowCount(), 1 );
    std::vector<double> expected_fpbm_fp {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    BOOST_CHECK_EQUAL_COLLECTIONS(fpbm->fingerprint().begin(), fpbm->fingerprint().end(), 
                              		expected_fpbm_fp.begin(), expected_fpbm_fp.end());



    // One over the DB
    BOOST_CHECK( !testDb.at(2) );

    BOOST_REQUIRE(testDb.write());

    BOOST_CHECK(checkFiles(copyOfTestDbFile));

}