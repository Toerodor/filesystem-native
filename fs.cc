#include <napi.h>
#include <experimental/filesystem>

#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <time.h>
#include <sys/stat.h>

#pragma comment(lib, "user32.lib")

#ifndef _DEBUG
#define DEBUG(x) 
#else
#define DEBUG(x) do { std::cerr << x << std::endl; } while (0)
#endif

namespace fs = std::experimental::filesystem;
using namespace Napi;

static int _timeZone = 0;

Value ConvertTimeToDate(Env env, time_t &time) {
	Object dateObj = Object::New(env);
	struct tm* ptm;

	ptm = gmtime(&time);
	int mon = ptm->tm_mon;
	if(mon + 1 > 12) 
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
	dateObj.Set("hour", (ptm->tm_hour + _timeZone) % 24);
	dateObj.Set("minuts", ptm->tm_min);
	dateObj.Set("seconds", ptm->tm_sec);

	return dateObj;
}

Value ChechOnFolder(const CallbackInfo& info) {
	Env env = info.Env();

	if (info.Length() < 1) {
		TypeError::New(env, "Missing arguments {path}").ThrowAsJavaScriptException();
		return env.Null();
	}

	std::string path = info[0].As<Napi::String>().ToString();
	
	try
	{
		if (fs::is_directory(path))
			return Boolean::New(env, true);
		else 
			return Boolean::New(env, false);
	}
	catch (fs::filesystem_error const& e)
	{
		return env.Null();
	}
}


Value InfoDiskInSystem(const CallbackInfo& info) {
	Env env = info.Env();
	//SYSTEM_INFO siSysInfo;
	//GetSystemInfo(&siSysInfo);

	Object sysInfo = Object::New(env);
	//sysInfo.Set("test", "");
	
	wchar_t disks[256];
	wchar_t* disk;
	DWORD sizebuf = 256;
	GetLogicalDriveStringsW(sizebuf, disks);
	disk = disks;

	while (*disk)
	{
		std::wstring ws(disk);
		std::string str(ws.begin(), ws.end());

		Object diskInf = Object::New(env);

		fs::space_info spaceInfo = fs::space(str);

		const uintmax_t available = spaceInfo.available;
		diskInf.Set("available", available);

		const uintmax_t capacity = spaceInfo.capacity;
		diskInf.Set("capacity", capacity);

		const uintmax_t free = spaceInfo.free;
		diskInf.Set("free", free);

		sysInfo.Set(str, diskInf);
		disk = disk + wcslen(disk) + 1;
	}


	return sysInfo;
}

Object InfoFile(Env env, fs::path filePath, int key) {
	Object fileInfo = Object::New(env);
	fileInfo.Set("key", key);

	const bool isDir = fs::is_directory(filePath);
	fileInfo.Set("isDir", isDir);

	const fs::path parentDir = filePath.parent_path();
	fileInfo.Set("parent", parentDir.u8string());

	const fs::path fileFullName = filePath.filename();
	fileInfo.Set("fullName", fileFullName.u8string());

	const fs::path fileName = filePath.stem();
	fileInfo.Set("name", fileName.u8string());

	if (!isDir) 
	{
		if (filePath.extension() == ".sys")
			return Object::New(env);

		const fs::path fileExtension = filePath.extension();
		fileInfo.Set("ext", fileExtension.u8string().erase(0,1));

		const int fileSize = fs::file_size(filePath);
		fileInfo.Set("size", fileSize);
	} 
	else 
	{
		fileInfo.Set("ext", "folder");
	}

	struct stat fileStatInfo;

	// file status
	if (stat(filePath.string().c_str(), &fileStatInfo) != 0) {
		// TODO: if error
	}

	fileInfo.Set("createTime", ConvertTimeToDate(env, fileStatInfo.st_ctime));
	fileInfo.Set("modifyTime", ConvertTimeToDate(env, fileStatInfo.st_mtime));

	const fs::path fileRootPath = filePath.root_path();
	fileInfo.Set("root", fileRootPath.string());

	return fileInfo;
}

Value DeleteFileFromPath(const CallbackInfo& info) {
	Env env = info.Env();
	if (info.Length() < 1) {
		TypeError::New(env, "Missing arguments {path}").ThrowAsJavaScriptException();
		return env.Null();
	}
	std::string path = info[0].As<Napi::String>().ToString();

	try 
	{
		fs::remove(fs::path(path));
		return Boolean::New(env, true);
	}
	catch (fs::filesystem_error const& e) 
	{
		return Boolean::New(env, false);
	}
}

Value DeleteFolderFromPath(const CallbackInfo& info) {
	Env env = info.Env();
	if (info.Length() < 1) {
		TypeError::New(env, "Missing arguments {path}").ThrowAsJavaScriptException();
		return env.Null();
	}
	std::string path = info[0].As<Napi::String>().ToString();
	
	try
	{
		fs::remove_all(fs::path(path));
		return Boolean::New(env, true);
	}
	catch (fs::filesystem_error const& e)
	{
		return Boolean::New(env, false);
	}
}

Value CreateFileFromPath(const CallbackInfo& info) {
	Env env = info.Env();
	if (info.Length() < 1) {
		TypeError::New(env, "Missing arguments {path}").ThrowAsJavaScriptException();
		return env.Null();
	}
	std::string path = info[0].As<Napi::String>().ToString();
	
	std::fstream file;
	file.open(path, std::ios::out);

	if (!file)
	{
		return Boolean::New(env, false);
	}

	file.close();

	return Boolean::New(env, true);
}

Value CreateFolderFromPath(const CallbackInfo& info) {
	Env env = info.Env();
	if (info.Length() < 1) {
		TypeError::New(env, "Missing arguments {path}").ThrowAsJavaScriptException();
		return env.Null();
	}
	std::string path = info[0].As<Napi::String>().ToString();
	
	try
	{
		fs::create_directory(path);
		return Boolean::New(env, true);
	}
	catch (fs::filesystem_error const& e)
	{
		return Boolean::New(env, false);
	}
}

Value RenameFile(const CallbackInfo& info) {
	Env env = info.Env();

	if (info.Length() < 1)
	{
		TypeError::New(env, "Missing arguments {pathFrom}").ThrowAsJavaScriptException();
		return env.Null();
	}
	else if (info.Length() < 2)
	{
		TypeError::New(env, "Missing arguments {pathTo}").ThrowAsJavaScriptException();
		return env.Null();
	}
	else if (info.Length() < 3)
	{
		TypeError::New(env, "Missing arguments {isOverwrite}").ThrowAsJavaScriptException();
		return env.Null();
	}

	std::string pathFrom = info[0].As<String>().ToString();
	std::string pathTo = info[1].As<String>().ToString();
	bool isOverwrite = info[3].As<Boolean>().IsBoolean();

	try
	{
		fs::rename(pathFrom, pathTo);
		return Boolean::New(env, true);
	}
	catch (fs::filesystem_error const& e)
	{
		return Boolean::New(env, false);
	}
}

Value RenameFolder(const CallbackInfo& info){
	Env env = info.Env();
	
	if (info.Length() < 1)
	{
		TypeError::New(env, "Missing arguments {path}").ThrowAsJavaScriptException();
		return env.Null();
	}
	else if (info.Length() < 2)
	{
		TypeError::New(env, "Missing arguments {newName}").ThrowAsJavaScriptException();
		return env.Null();
	}
	else if (info.Length() < 3)
	{
		TypeError::New(env, "Missing arguments {isOverwrite}").ThrowAsJavaScriptException();
		return env.Null();
	}

	std::string path = info[0].As<String>().ToString();
	std::string newName = info[1].As<String>().ToString();
	bool isOverwrite = info[3].As<Boolean>().IsBoolean();

	try
	{
		if (isOverwrite)
		{
			fs::copy(path, newName, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
			fs::remove_all(path);
		}
		else
		{
			fs::copy(path, newName, fs::copy_options::recursive | fs::copy_options::skip_existing);
			fs::remove_all(path);
		}

		return Boolean::New(env, true);
	}
	catch (fs::filesystem_error const& e)
	{
		return Boolean::New(env, false);
	}

}

Value CopyFileFromPath(const CallbackInfo& info) {
	Env env = info.Env();

	if (info.Length() < 1)
	{
		TypeError::New(env, "Missing arguments {pathFrom}").ThrowAsJavaScriptException();
		return env.Null();
	}
	else if (info.Length() < 2)
	{
		TypeError::New(env, "Missing arguments {pathTo}").ThrowAsJavaScriptException();
		return env.Null();
	}
	else if (info.Length() < 3)
	{
		TypeError::New(env, "Missing arguments {isOverwrite}").ThrowAsJavaScriptException();
		return env.Null();
	}

	std::string pathFrom = info[0].As<String>().ToString();
	std::string pathTo = info[1].As<String>().ToString();
	bool isOverwrite = info[3].As<Boolean>().IsBoolean();

	try
	{
		if (isOverwrite)
		{
			fs::copy(pathFrom, pathTo, fs::copy_options::overwrite_existing);
		}
		else
		{
			fs::copy(pathFrom, pathTo);
		}

		return Boolean::New(env, true);
	}
	catch (fs::filesystem_error const& e)
	{
		return Boolean::New(env, false);
	}
}

Value CopyFolderFromPath(const CallbackInfo& info) {
	Env env = info.Env();

	if (info.Length() < 1) 
	{
		TypeError::New(env, "Missing arguments {pathFrom}").ThrowAsJavaScriptException();
		return env.Null();
	}
	else if (info.Length() < 2) 
	{
		TypeError::New(env, "Missing arguments {pathTo}").ThrowAsJavaScriptException();
		return env.Null();
	}
	else if (info.Length() < 3) 
	{
		TypeError::New(env, "Missing arguments {isOverwrite}").ThrowAsJavaScriptException();
		return env.Null();
	}

	std::string pathFrom = info[0].As<String>().ToString();
	std::string pathTo = info[1].As<String>().ToString();
	bool isOverwrite = info[3].As<Boolean>().IsBoolean();

	try
	{
		if (isOverwrite)
		{
			fs::copy(pathFrom, pathTo, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
		}
		else
		{
			fs::copy(pathFrom, pathTo, fs::copy_options::recursive | fs::copy_options::skip_existing);
		}

		return Boolean::New(env, true);
	}
	catch (fs::filesystem_error const& e)
	{
		return Boolean::New(env, false);
	}
}

Value InfoFolderFromPath(const CallbackInfo& info) {
	Env env = info.Env();

	 if (info.Length() < 1) {
	 	TypeError::New(env, "Missing arguments {path}").ThrowAsJavaScriptException();
	 	return env.Null();
	 }

	 if (info.Length() < 2) {
		 TypeError::New(env, "Missing arguments {timeZone}").ThrowAsJavaScriptException();
		 return env.Null();
	 }

	std::string arg0 = info[0].As<Napi::String>().ToString();
	fs::path filePath = fs::path(arg0);

	_timeZone = info[1].As<Napi::Number>().ToNumber();

	try 
	{
		if (fs::is_directory(filePath)) {
			int index = 0;
			Object arr = Object::New(env);

			for (auto& p : fs::directory_iterator(filePath)) {	
				arr.Set(index, InfoFile(env, p.path(), index));
				index++;
			}

			return arr;
		}
		
		return String::New(env, "Error");
	}
	catch (...)
	{
		return String::New(env, "Error");
	}
}

String Method(const CallbackInfo& info) {
  Env env = info.Env();
  fs::path path = fs::current_path();
  
  return String::New(env, path.string());
}

Object Init(Env env, Object exports) {
  exports.Set(String::New(env, "hello"),
              Function::New(env, Method));

  exports.Set(String::New(env, "folderInfo"),
			  Function::New(env, InfoFolderFromPath));

  exports.Set(String::New(env, "diskInfo"),
			  Function::New(env, InfoDiskInSystem));

  exports.Set(String::New(env, "deleteFile"),
			  Function::New(env, DeleteFileFromPath));

  exports.Set(String::New(env, "deleteFolder"),
			  Function::New(env, DeleteFolderFromPath));

  exports.Set(String::New(env, "createFolder"),
			  Function::New(env, CreateFolderFromPath));

  exports.Set(String::New(env, "createFile"),
			  Function::New(env, CreateFileFromPath));

  exports.Set(String::New(env, "copyFolder"),
			  Function::New(env, CopyFolderFromPath));

  exports.Set(String::New(env, "copyFile"),
			  Function::New(env, CopyFileFromPath));

  exports.Set(String::New(env, "renameFile"),
			  Function::New(env, RenameFile));

  exports.Set(String::New(env, "renameFolder"),
			  Function::New(env, RenameFolder));

  exports.Set(String::New(env, "isFolder"),
			  Function::New(env, ChechOnFolder));

  return exports;
}

NODE_API_MODULE(addon, Init)
