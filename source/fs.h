#include <napi.h>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
using namespace Napi;

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

namespace filesystem_h
{
  class native_fs 
  {

    public:
      static Value ConvertTimeToDate(Env env, time_t &time, int timeZone) {
        Object dateObj = Object::New(env);
        struct tm *ptm;

        ptm = gmtime(&time);
        int mon = ptm->tm_mon;

        if (mon + 1 > 12)
        {
          mon = 1;
        }
        else
        {
          mon = mon + 1;
        }

        dateObj.Set("year", ptm->tm_year + 1900);
        dateObj.Set("month", mon);
        dateObj.Set("day", ptm->tm_mday);
        dateObj.Set("hour", (ptm->tm_hour + timeZone) % 24);
        dateObj.Set("minuts", ptm->tm_min);
        dateObj.Set("seconds", ptm->tm_sec);

        return dateObj;
      };

      Value CheckOnFolder(const CallbackInfo &info);

      Value InfoDiskInSystem(const CallbackInfo &info);

      Value InfoFile(Env env, fs::path filePath, int key);

      Value DeleteFileFromPath(const CallbackInfo &info);

      Value DeleteFolderFromPath(const CallbackInfo &info);

      Value CreateFileFromPath(const CallbackInfo &info);

      Value CreateFolderFromPath(const CallbackInfo &info);

      Value RenameFile(const CallbackInfo &info);

      Value RenameFolder(const CallbackInfo &info);

      Value CopyFileFromPath(const CallbackInfo &info);

      Value CopyFolderFromPath(const CallbackInfo &info);

      Value InfoFolderFromPath(const CallbackInfo &info);
  };

}



/* FILESYSTEM_H: */
#endif