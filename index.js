var addon = require('bindings')('addon');

module.exports = {
	/**
	 * Copy file or folder
	 * @param {boolean} isDir 
	 * @param {string} pathFrom 
	 * @param {string} pathTo 
	 * @param {boolean} override 
	 * @returns {boolean}
	 */
  copy: function (isDir, pathFrom, pathTo, override) {
    if (isDir) {
      return addon.copyFolder(pathFrom, pathTo, override)
    } else {
      return addon.copyFile(pathFrom, pathTo, override)
    }
  },

	/**
	 * Create folder 
	 * @param {string} path 
	 * @param {boolean} isDir 
	 * @returns {boolean}
	 */
  create: function (path, isDir = true) {
    if (isDir) {
      return addon.createFolder(path)
    } else {
      // return addon.copyFile(pathFrom, pathTo, override)
    }
  },

	/**
	 * Rename file
	 * @param {string} path 
	 * @param {string} newName 
	 */
  rename: function (path, newName) {
    return addon.renameFile(path, newName, true)
  },

	/**
	 * Delete file or folder
	 * @param {string} path 
	 * @param {boolean} isDir 
	 */
  delete: function (path, isDir) {
    if (isDir) {
      return addon.deleteFolder(path)
    } else {
      return addon.deleteFile(path)
    }
  },

	/**
	 * Scan folder and return object with files
	 * @param {string} path 
	 */
  scanDir: function (path) {
    return addon.folderInfo(path)
  }
};