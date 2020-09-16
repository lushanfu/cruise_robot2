/*
 * @Author: LSF
 * @Date: 2020-09-16 16:35:38
 * @LastEditTime: 2020-09-16 16:44:17
 * @LastEditors: Please set LastEditors
 * @Description: camera control module ,如：拍照 、旋转相机角度、变焦、照片存储
 */
class CCamera
{
private:
    /* data */
public:
    CCamera(/* args */);
    ~CCamera();

public:
    void takePhotos();
    void rotate2FixedAnchor();
    void zoom();
    void imgSave();

};
