# filesystem-native
Filesystem for node js write on c++

## Usage
```javascript
import fs from 'native-filesystem-node'

# {string} path = "C:\\"; {number} timeZone = +1; 
let object = fs.scanDir(path, timeZone); 

# {string} path = "C:\\"; {boolean} isDir = true; 
fs.delete(path, isDir);

# {string} pathFrom = "C:\\folder"; {string} pathTo = "C:\\newFolder"; 
# {boolean} isDir = true; {boolean} override = true;
fs.copy(isDir, pathFrom, pathTo, override);

# {string} path = "C:\\folder"; {string} newName = "C:\\newFolder"; 
# {boolean} isDir = true; {boolean} override = true;
fs.rename(isDir, path, newName, override);

# {string} path = "C:\\folder";   
fs.create(path);

```

## License
[MIT](https://choosealicense.com/licenses/mit/)