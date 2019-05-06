var addon = require('bindings')('addon');

module.exports = {
	/**
	 * Copy file or folder
	 * @param {string} pathFrom 
	 * @param {string} pathTo 
	 * @param {boolean} override 
	 * @returns {boolean}
	 */
  copy: function (pathFrom, pathTo, override) {
    let isDir = addon.isFolder(path);
    if (isDir) {
      return addon.copyFolder(pathFrom, pathTo, override)
    } else {
      return addon.copyFile(pathFrom, pathTo, override)
    }
  },

	/**
	 * Create folder 
	 * @param {string} path 
	 * @returns {boolean}
	 */
  create: function (path) {
    let isDir = addon.isFolder(path);
    if (isDir) {
      return addon.createFolder(path)
    } else {
      return addon.createFile(path)
    }
  },

	/**
	 * Rename file
	 * @param {string} path 
	 * @param {string} newName 
	 */
  rename: function (path, newName, override) {
    let isDir = addon.isFolder(path);
    if (isDir) {
        return addon.renameFolder(path, newName, override)
    } else {
        return addon.renameFile(path, newName, override)
    }
  },

	/**
	 * Delete file or folder
	 * @param {string} path 
	 */
  delete: function (path) {
    let isDir = addon.isFolder(path);
    if (isDir) {
      return addon.deleteFolder(path)
    } else {
      return addon.deleteFile(path)
    }
  },

	/**
	 * Scan folder and return object with files
	 * @param {string} path
	 * @param {number} timeZone
	 */
  scanDir: function (path, timeZone) {
    let isDir = addon.isFolder(path);
    if(isDir) {
      let obj = addon.folderInfo(path, timeZone);
      return Object.keys(obj).map(function (key) {
        return obj[key];
      });
    } else {
      return [];
    }
  },

  diskInfo: function () {
    let obj = addon.diskInfo();
    return Object.keys(obj).map(function (key) {
      return obj[key];
    });
  }
};