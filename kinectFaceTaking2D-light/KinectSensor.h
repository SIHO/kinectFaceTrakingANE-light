// Thaks : http://www.codeproject.com/Articles/394975/How-To-Use-Kinect-Face-Tracking-SDK
#pragma once 

// NuiApi.hの前にWindows.hをインクルードする
#define _USE_MATH_DEFINES
#include <Windows.h>
#include <NuiApi.h>
#include <math.h>
#include <FaceTrackLib.h>
#include "FlashRuntimeExtensions.h"

#include <iostream>
#include <sstream>
#include <vector>


#define ERROR_CHECK( ret )  \
    if ( ret != S_OK ) {    \
    std::stringstream ss;	\
    ss << "failed " #ret " " << std::hex << ret << std::endl;			\
    throw std::runtime_error( ss.str().c_str() );			\
    }

const NUI_IMAGE_RESOLUTION CAMERA_RESOLUTION = NUI_IMAGE_RESOLUTION_640x480;

#define SAFE_RELEASE( p ) if ( p != 0 ) { p->Release(); p = 0; }

class KinectSensor
{
private:

    INuiSensor* kinect;
    HANDLE imageStreamHandle;
    HANDLE depthStreamHandle;
    HANDLE streamEvent;

    DWORD width;
    DWORD height;

    IFTFaceTracker* pFT;                // 顔追跡する人
    FT_CAMERA_CONFIG videoCameraConfig; // RGBカメラの設定
    FT_CAMERA_CONFIG depthCameraConfig; // 距離カメラの設定

    FT_VECTOR3D hint3D[2];

    IFTResult* pFTResult;               // 顔追跡結果

    IFTImage* pColorFrame;              // 顔追跡用のRGBデータ
    IFTImage* pDepthFrame;              // 顔追跡用の距離データ

    FT_SENSOR_DATA sensorData;          // Kinectセンサーデータ

    std::vector<unsigned char> colorCameraFrameBuffer;
    std::vector<unsigned char> depthCameraFrameBuffer;

    RECT faceRect;
    //顔追跡されているかどうか
    bool isFaceTracked;

public:

    KinectSensor()
        : pFT( 0 )
        , pFTResult( 0 )
        , isFaceTracked( false )
    {
    }

    ~KinectSensor()
    {
		close();
    }

	void close()
	{
        // 終了処理
        if ( kinect != 0 ) {
			SAFE_RELEASE( pFTResult );

			SAFE_RELEASE( pColorFrame );
			SAFE_RELEASE( pDepthFrame );
			SAFE_RELEASE( pFT );

			if ( kinect != 0 ) {
				kinect->NuiShutdown();
				SAFE_RELEASE( kinect );
			}
        }
	}

    void initialize()
    {
        NuiSetDeviceStatusCallback( StatusProc, 0 );

        createInstance();
        // Kinectの設定を初期化する
        ERROR_CHECK( kinect->NuiInitialize(
            NUI_INITIALIZE_FLAG_USES_COLOR |
            NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX |
            NUI_INITIALIZE_FLAG_USES_SKELETON ) );

        // RGBカメラを初期化する
        ERROR_CHECK( kinect->NuiImageStreamOpen( NUI_IMAGE_TYPE_COLOR, CAMERA_RESOLUTION,
            0, 2, 0, &imageStreamHandle ) );

        // 距離カメラを初期化する
        ERROR_CHECK( kinect->NuiImageStreamOpen( NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, CAMERA_RESOLUTION,
            NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE, 2, 0, &depthStreamHandle ) );

        // Nearモード Xboxのときはここをオフにする
        ERROR_CHECK( kinect->NuiImageStreamSetImageFrameFlags(
            depthStreamHandle, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE ) );

        // スケルトンを初期化する Xboxのときはここをオフにする
        ERROR_CHECK( kinect->NuiSkeletonTrackingEnable( 0,
            NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE | 
            NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT ) );

        // フレーム更新イベントのハンドルを作成する
        streamEvent = ::CreateEvent( 0, TRUE, FALSE, 0 );
        ERROR_CHECK( kinect->NuiSetFrameEndEvent( streamEvent, 0 ) );

        // 指定した解像度の、画面サイズを取得する
        ::NuiImageResolutionToSize(CAMERA_RESOLUTION, width, height );

        // 顔認識の初期化
        initializeFaceTracker();
    }

    void initialize2()
    {

        createInstance();
        // Kinectの設定を初期化する
        ERROR_CHECK( kinect->NuiInitialize(
            NUI_INITIALIZE_FLAG_USES_COLOR |
            NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX |
            NUI_INITIALIZE_FLAG_USES_SKELETON ) );

        // RGBカメラを初期化する
        ERROR_CHECK( kinect->NuiImageStreamOpen( NUI_IMAGE_TYPE_COLOR, CAMERA_RESOLUTION,
            0, 2, 0, &imageStreamHandle ) );

        // 距離カメラを初期化する
        ERROR_CHECK( kinect->NuiImageStreamOpen( NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, CAMERA_RESOLUTION,
            0, 2, 0, &depthStreamHandle ) );

        // Nearモード Xboxのときはここをオフにする
        //ERROR_CHECK( kinect->NuiImageStreamSetImageFrameFlags(
        //     depthStreamHandle, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE ) );

        // スケルトンを初期化する Xboxのときはここをオフにする
        ERROR_CHECK( kinect->NuiSkeletonTrackingEnable( 0,
            //     NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE | 
            NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT ) );

        // フレーム更新イベントのハンドルを作成する
        streamEvent = ::CreateEvent( 0, TRUE, FALSE, 0 );
        ERROR_CHECK( kinect->NuiSetFrameEndEvent( streamEvent, 0 ) );

        // 指定した解像度の、画面サイズを取得する
        ::NuiImageResolutionToSize(CAMERA_RESOLUTION, width, height );

        // 顔認識の初期化
        initializeFaceTracker();
    }

    void initializeFaceTracker()
    {
        // FaceTrackerのインスタンスを生成する
        pFT = ::FTCreateFaceTracker();
        if( !pFT ) {
            throw std::runtime_error( "ERROR:FTCreateFaceTracker" );
        }

        // RGBカメラおよび距離カメラの設定を行う
        videoCameraConfig.Width = width;
        videoCameraConfig.Height = height;
        videoCameraConfig.FocalLength =
            NUI_CAMERA_COLOR_NOMINAL_FOCAL_LENGTH_IN_PIXELS * (width / 640.0f);

        depthCameraConfig.Width = width;
        depthCameraConfig.Height = height;
        depthCameraConfig.FocalLength = 
            NUI_CAMERA_DEPTH_NOMINAL_FOCAL_LENGTH_IN_PIXELS * (width / 320.0f);

        // FaceTrackerを初期化する
        HRESULT hr = pFT->Initialize( &videoCameraConfig, &depthCameraConfig, 0, 0) ;
        if( FAILED(hr) ) {
            throw std::runtime_error( "ERROR:Initialize" );
        }

        // FaceTrackerの結果を格納先を生成する
		if( !pFTResult ) {
			//
			hr = pFT->CreateFTResult( &pFTResult );
			if(FAILED(hr)) {
			  throw std::runtime_error( "ERROR:CreateFTResult" );
			}
            //
        }

        // FaceTrackerで利用するRGBおよび距離データのインスタンスを生成する
        pColorFrame = FTCreateImage();
        pDepthFrame = FTCreateImage();
        if( !pColorFrame || !pDepthFrame ) {
            throw std::runtime_error( "ERROR:FTCreateImage" );
        }

        // RGBおよび距離データのバッファサイズを設定する
        // RGBは1pixelあたり4バイト。距離は1pixelあたり2バイト
        colorCameraFrameBuffer.resize( width*4 * height );
        depthCameraFrameBuffer.resize( width*2 * height );

        // フレームデータにバッファを設定する
        // You can also use Allocate() method in which case IFTImage interfaces own their memory.
        // In this case use CopyTo() method to copy buffers
        // CopyToでもOK?
        pColorFrame->Attach(width, height, &colorCameraFrameBuffer[0], FTIMAGEFORMAT_UINT8_B8G8R8X8, width*4);
        pDepthFrame->Attach(width, height, &depthCameraFrameBuffer[0], FTIMAGEFORMAT_UINT16_D13P3, width*2);

        // センサーデータを作成する
        sensorData.pVideoFrame = pColorFrame;
        sensorData.pDepthFrame = pDepthFrame;
        sensorData.ZoomFactor = 1.0f;			// Not used must be 1.0
        sensorData.ViewOffset.x = 0;			// Not used must be (0,0)
        sensorData.ViewOffset.y = 0;			// Not used must be (0,0)
    }

	void update()
    {
        // データの更新を200ミリ秒だけ待つ
        DWORD ret = ::WaitForSingleObject( streamEvent, 200 );
        ::ResetEvent( streamEvent );

		if(ret == WAIT_TIMEOUT)return;

        copyStreamData( imageStreamHandle, colorCameraFrameBuffer );
        copyStreamData( depthStreamHandle, depthCameraFrameBuffer );
        copySkeleton();

        faceTracking();
    }

    const std::vector<unsigned char>& getColorFrameData() const
    {
        return colorCameraFrameBuffer;
    }

    const std::vector<unsigned char>& getDepthFrameData() const
    {
        return depthCameraFrameBuffer;
    }

    bool IsFaceTracked() const
    {
        return isFaceTracked;
    }

    const RECT& FaceRect()
    {
        return faceRect;
    }

    DWORD getWidth() const
    {
        return width;
    }

    DWORD getHeight() const
    {
        return height;
    }

private:

    static void CALLBACK StatusProc( HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName, void* pUserData)
    {
    }

    void createInstance()
    {
        // 接続されているKinectの数を取得する
        int count = 0;
        ERROR_CHECK( ::NuiGetSensorCount( &count ) );
        if ( count == 0 ) {
            throw std::runtime_error( "Kinect を接続してください" );
        }

        // 最初のKinectのインスタンスを作成する
        ERROR_CHECK( ::NuiCreateSensorByIndex( 0, &kinect ) );

        // Kinectの状態を取得する
        HRESULT status = kinect->NuiStatus();
        if ( status != S_OK ) {
            throw std::runtime_error( "Kinect が利用可能ではありません" );
        }
    }

    void copyStreamData( HANDLE streamHandle, std::vector<unsigned char>& buffer )
    {
        // RGBカメラのフレームデータを取得する
        NUI_IMAGE_FRAME frame = { 0 };
        ERROR_CHECK( kinect->NuiImageStreamGetNextFrame( streamHandle, INFINITE, &frame ) );

        // 画像データを取得する
        NUI_LOCKED_RECT data;
        frame.pFrameTexture->LockRect( 0, &data, 0, 0 );

        // データをコピーする
        if ( data.size != buffer.size() ) {
            buffer.resize( data.size );
        }

        std::copy( data.pBits, data.pBits + data.size, buffer.begin() );

        // フレームデータを解放する
        ERROR_CHECK( kinect->NuiImageStreamReleaseFrame( streamHandle, &frame ) );
    }

    void copySkeleton()
    {
        // スケルトンのフレームを取得する
        NUI_SKELETON_FRAME skeletonFrame = { 0 };
        kinect->NuiSkeletonGetNextFrame( 0, &skeletonFrame );
        for ( int i = 0; i < NUI_SKELETON_COUNT; ++i ) {
            NUI_SKELETON_DATA& skeletonData = skeletonFrame.SkeletonData[i];
            if ( skeletonData.eTrackingState == NUI_SKELETON_TRACKED ) {
                // 頭と首の位置をヒントとして、FaceTrackingに渡す
                if ( (skeletonData.eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_HEAD] != NUI_SKELETON_POSITION_NOT_TRACKED) &&
                     (skeletonData.eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_SHOULDER_CENTER] != NUI_SKELETON_POSITION_NOT_TRACKED) ) {
                        // pHint3D[0] = m_NeckPoint[selectedSkeleton];
                         hint3D[0].x = skeletonData.SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER].x;
                         hint3D[0].y = skeletonData.SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER].y;
                         hint3D[0].z = skeletonData.SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER].z;

                        // pHint3D[1] = m_HeadPoint[selectedSkeleton];
                         hint3D[1].x = skeletonData.SkeletonPositions[NUI_SKELETON_POSITION_HEAD].x;
                         hint3D[1].y = skeletonData.SkeletonPositions[NUI_SKELETON_POSITION_HEAD].y;
                         hint3D[1].z = skeletonData.SkeletonPositions[NUI_SKELETON_POSITION_HEAD].z;
                }
                else {
                    hint3D[0] = hint3D[1] = FT_VECTOR3D(0, 0, 0);
                }

                break;
            }
        }
    }

    std::vector<unsigned char> copyStreamData( HANDLE streamHandle )
    {
        std::vector<unsigned char> buffer;
        copyStreamData( streamHandle, buffer );
        return buffer;
    }

private:

    // 顔追跡(内部用)
    HRESULT faceTracking_p()
    {
        // 追跡中、未追跡によって処理が変わる
        if(!isFaceTracked) {
            // FaceTrakingを開始する
            return pFT->StartTracking(&sensorData, NULL, hint3D, pFTResult);
        }
        else {
            // FaceTrakingを継続する
            return pFT->ContinueTracking(&sensorData, hint3D, pFTResult);
        }
    }

private:

    FT_VECTOR2D* points;
    UINT pointCount;

public:

    FT_VECTOR2D* get2DPoints()const
    {
        return points;
    }

    UINT get2DPointCount() const
    {
        return pointCount;
    }

    // 顔追跡(公開用)
    void faceTracking()
    {
        // 顔追跡
        HRESULT hr = faceTracking_p();
        // 顔を見つけたので、追跡状態へ遷移
        if(SUCCEEDED(hr) && SUCCEEDED(pFTResult->GetStatus())) {
            ::OutputDebugString( L"FaceTracking Success\n" );
            isFaceTracked = true;

            // 顔の領域を取得する
            pFTResult->GetFaceRect( &faceRect );

            // 2Dの座標を取得する
            points = 0;
            pointCount = 0;
            hr = pFTResult->Get2DShapePoints( &points, &pointCount );
            if( FAILED(hr) ) {
                ::OutputDebugString( L"Get2DShapePoints Failed\n" );
            }
        }
        // 顔を見失ったので、未追跡状態のまま
        else {
            ::OutputDebugString( L"FaceTracking failed\n" );
            isFaceTracked = false;
        }
    }

    //読み取りだけのプロパティ
    //kinectが有効かどうか
	bool actKinect;

    //設定可能なプロパティ
    //Kinectの上下角を顔の鼻の位置を中心として追従すかどうか
    uint32_t moveTiltMortor;
    //FaceTrakingをするかどうか
    uint32_t doFaceTraking ;

    //ANE用Func
    //関数Kinect内での実行のみ
    void moveTiltAlignment(){

        if(IsFaceTracked()){
            //顔の識別点を取得
            const FT_VECTOR2D* points = get2DPoints();
            //鼻のY座標。 points[89].y;
            double w =points[89].x;
            double h =points[89].y;

            int n = 640 * int(h+0.5) + int(w+0.5) ;
            //std::cout<< "/////////////////////////////////\n moveTiltMortor:NosePoint w & h  " << w<< ","<< h<< "/n" <<n<< std::endl;

            // 距離カメラのフレームデータを取得する
            NUI_IMAGE_FRAME depthFrame = { 0 };
            ERROR_CHECK( kinect->NuiImageStreamGetNextFrame( depthStreamHandle, INFINITE, &depthFrame ) );

            // 距離データを取得する
            NUI_LOCKED_RECT depthData = { 0 };
            depthFrame.pFrameTexture->LockRect( 0, &depthData, 0, 0 );

            // フレームデータを解放する
            ERROR_CHECK( kinect->NuiImageStreamReleaseFrame( depthStreamHandle, &depthFrame ) );

            USHORT* depth = (USHORT*)depthData.pBits;
            double centerD = ::NuiDepthPixelToDepth( depth[640*240+320] );
            if(centerD>1000)return;
            double noseD = ::NuiDepthPixelToDepth( depth[n] );

            if(noseD>0 && centerD>0){
                //std::cout<< "moveTiltMortor:NoseD" << noseD << std::endl;
                //std::cout<<"centerDistance" << centerD << std::endl;

                double acosVal;

                noseD> centerD ? acosVal = acos(centerD/noseD): acosVal = acos(noseD/centerD);
                //std::cout<< "moveTiltMortor:acos " << acosVal << std::endl;
                acosVal = (acosVal * 180/M_PI)/2;
                //std::cout<< "moveTiltMortor:acosM_PI " << acosVal << std::endl;

                LONG angle = 0;
                kinect->NuiCameraElevationGetAngle( &angle );

                //std::cout<< "moveTiltMortor:angleDef " << angle << std::endl;
                if(acosVal>=5){
                    //std::cout<< "moveTiltMortor:points[89].y " << h << std::endl;
                    if(h < 240){
                        angle = angle +  acosVal;
                        //std::cout<< "moveTiltMortor:angle " << angle << std::endl;
                        angle>(LONG)NUI_CAMERA_ELEVATION_MAXIMUM ? angle = (LONG)NUI_CAMERA_ELEVATION_MAXIMUM:angle = angle;
                        //angle = std::min( angle, (LONG)NUI_CAMERA_ELEVATION_MAXIMUM );
                    }else {
                        angle = angle +  acosVal*-1;
                        //std::cout<< "moveTiltMortor:angle " << angle << std::endl;

                        angle >(LONG)NUI_CAMERA_ELEVATION_MINIMUM ? angle = angle:angle=(LONG)NUI_CAMERA_ELEVATION_MINIMUM;

                        //angle = std::max( angle, (LONG)NUI_CAMERA_ELEVATION_MINIMUM );
                    }
                    //std::cout<< "moveTiltMortor:angle2 " << angle << std::endl;
                    kinect->NuiCameraElevationSetAngle( angle );
                }
            }
        }
    }

    //顔追跡をするかどうかを切り替える
    bool aneDoFaceTraking(){
        doFaceTraking ? doFaceTraking =false:doFaceTraking = true;
        std::cout << "doFaceTraking:"<<doFaceTraking<<std::endl;
    }
};

//<-----------------------ane通信----------------------------------->//


extern "C" __declspec(dllexport) void ExtInitializer(
    void** extDataToSet,
    FREContextInitializer* ctxInitializerToSet,
    FREContextFinalizer* ctxFinalizerToSet
    );

extern "C" __declspec(dllexport) void ExtFinalizer(
    void* extData
    );

KinectSensor kinect;

// ネイティブ関数の本体
//return bool
FREObject aneInitKinect(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
    //std::cout << "aneInitKinect" << std::endl;

    uint32_t funcBool;//nearMode
    FREGetObjectAsBool(argv[0], &funcBool);

    kinect.moveTiltMortor = false;
    kinect.doFaceTraking = false;

	//!funcBool? kinect.initialize2(): kinect.initialize();

    try {
        !funcBool? kinect.initialize2(): kinect.initialize();
        //kinect.initialize();
        kinect.actKinect= true;
    } catch ( std::exception& ex ) {
        kinect.actKinect = false;
        std::cout << ex.what() << std::endl;
    }


   uint32_t resbool =kinect.actKinect;

    FREObject result; 
    FRENewObjectFromBool(resbool, &result);
    //std::cout << "kinect.actKinect" <<kinect.actKinect<< std::endl;
    return result;
}
//kinectのUpdate
//return bool
FREObject aneStartUpdate(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
    //argv[0] moveTiltMortor true/false 
    uint32_t funcBool;
    FREGetObjectAsBool(argv[0], &funcBool);
    //std::cout << "aneDoFaceTraking:moveTiltMortor " <<funcBool << std::endl;
    kinect.moveTiltMortor = funcBool;

    if(!kinect.doFaceTraking)kinect.doFaceTraking = true;
    //std::cout << "aneStartUpdate" << std::endl;
    if(kinect.doFaceTraking){
        kinect.update();
        if(kinect.moveTiltMortor)kinect.moveTiltAlignment();
    }

    uint32_t resbool =kinect.doFaceTraking;

    FREObject result; 
    FRENewObjectFromBool(resbool, &result);
    return result;
}

// "Hello, World!" という文字列データをFREObject値として返す
FREObject hello(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) 
{

    const uint8_t* msg = (const uint8_t*)"Hello World";

    FREObject retObj;
    //文字列からFREObjectを作る関数でASで扱える文字列にする
    FRENewObjectFromUTF8(strlen((const char*)msg)+1, msg, &retObj);

    return retObj;
}
//return Vector.<Point>
FREObject aneDoFaceTraking(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]){
    FREObject arrayPoint;
    FRENewObject( (const uint8_t*) "Vector.<flash.geom.Point>", 0, NULL, &arrayPoint, NULL);
    uint32_t arr_len;
    FREObject resPoint;
    FREObject freX, freY;
    if ( kinect.IsFaceTracked() ) {
        arr_len = kinect.get2DPointCount();
        FRESetArrayLength(arrayPoint , arr_len);
        const FT_VECTOR2D* points = kinect.get2DPoints();
        for ( unsigned int i = 0; i < arr_len; ++i ) {
            double x = points[i].x;
            double y  = points[i].y;

            FRENewObjectFromDouble(x, &freX);
            FRENewObjectFromDouble(y, &freY);
            FREObject pointParams[] = {freX, freY};

            FRENewObject( (const uint8_t*) "flash.geom.Point", 2, pointParams, &resPoint, NULL);
            //std::cout <<i << " aneDoFaceTrakingresPoint " <<resPoint << std::endl;
            FRESetArrayElementAt(arrayPoint, i, resPoint);
        }
        //std::cout << "Going through the vector: " <<kinect.get2DPointCount()<< std::endl;
    }else {
        arr_len = 1;
        FRESetArrayLength(arrayPoint , arr_len);

        double x = 0.0;
        double y  =0.0;

        FRENewObjectFromDouble(x, &freX);
        FRENewObjectFromDouble(y, &freY);
        FREObject pointParams[] = {freX, freY};
        FRENewObject( (const uint8_t*) "flash.geom.Point", 0, pointParams, &resPoint, NULL);

        FRESetArrayElementAt(arrayPoint, 0, resPoint);
    }
    return arrayPoint;
}
//return Array at length is 4;
FREObject aneRectArray(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]){
    uint32_t arr_len = 4;
    FREObject populatedArray = NULL;
    FRENewObject((const uint8_t*)"Array", 0, NULL, &populatedArray, NULL);  
    FRESetArrayLength(populatedArray, arr_len);

    //std::cout << "aneRectArray:kinect.IsFaceTracked " <<kinect.IsFaceTracked() << std::endl;
    if ( kinect.IsFaceTracked() ) {
        //std::cout << "aneRectArray:kinect.FaceRect()bottom " <<kinect.FaceRect().bottom << std::endl;

        double x = kinect.FaceRect().left;
        double y  = kinect.FaceRect().top;
        double width = kinect.FaceRect().right - kinect.FaceRect().left;
        double height = kinect.FaceRect().bottom - kinect.FaceRect().top;

        FREObject freX, freY, freW, freH;
        FRENewObjectFromDouble(x, &freX);
        FRENewObjectFromDouble(y, &freY);
        FRENewObjectFromDouble(width, &freW);
        FRENewObjectFromDouble(height, &freH);

        FRESetArrayElementAt(populatedArray, 0, freX);
        FRESetArrayElementAt(populatedArray, 1, freY);
        FRESetArrayElementAt(populatedArray, 2, freW);
        FRESetArrayElementAt(populatedArray, 3, freH);
    }else {
        double n = 0.0;FREObject freN;
        FRENewObjectFromDouble(n, &freN);
        FRESetArrayElementAt(populatedArray, 0, freN);
        FRESetArrayElementAt(populatedArray, 1, freN);
        FRESetArrayElementAt(populatedArray, 2, freN);
        FRESetArrayElementAt(populatedArray, 3, freN);
    }
    return populatedArray;
}

// 拡張コンテキスト初期化時に呼ばれる
void ContextInitializer( void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctions, const FRENamedFunction** functionsToSet) {
    *numFunctions = 5; //関数定義の数

    //関数定義の配列を作る
    FRENamedFunction* func =
        (FRENamedFunction*)malloc(sizeof(FRENamedFunction)*(*numFunctions)); 

    func[0].name = (const uint8_t*)"hello"; //関数名
    func[0].functionData = NULL; //関数データ
    func[0].function = &hello; //呼ばれる関数のFREFunctionポインタ 

    //増やす時　numFunctionsも増やす
    func[1].name = (const uint8_t*) "aneInitKinect";
    func[1].functionData = NULL;
    func[1].function = &aneInitKinect;

    func[2].name = (const uint8_t*) "aneDoFaceTraking";
    func[2].functionData = NULL;
    func[2].function = &aneDoFaceTraking;

    func[3].name = (const uint8_t*) "aneRectArray";
    func[3].functionData = NULL;
    func[3].function = &aneRectArray;

    func[4].name = (const uint8_t*) "aneStartUpdate";
    func[4].functionData = NULL;
    func[4].function = &aneStartUpdate;

    *functionsToSet = func;
}

// 拡張コンテキスト破棄時に呼ばれる
void ContextFinalizer(FREContext ctx) {
	kinect.close();
    return;
}

// アプリケーション初期化時に呼ばれる, DLLからエクスポート
__declspec(dllexport) void ExtInitializer(
    void** extDataToSet,
    FREContextInitializer* ctxInitializerToSet,
    FREContextFinalizer* ctxFinalizerToSet
    ) {
        *extDataToSet = NULL;
        *ctxInitializerToSet = &ContextInitializer; //コンテキストの初期化時の関数
        *ctxFinalizerToSet = &ContextFinalizer; //コンテキストの破棄時の関数
}
// アプリケーション終了時に呼ばれる, DLLからエクスポート
__declspec(dllexport) void ExtFinalizer(void* extData) {
    return;
}