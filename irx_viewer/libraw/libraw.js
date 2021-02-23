const sharp = require('sharp')

const raw = require('./build/Release/node_libraw')

const libraw = {
  extract: function (path) {
    return new Promise(function (resolve, reject) {
      raw.extract(path, function (err, data, width, height) {
        if (err) {
          return reject(err)
        }
        // NOTE(amiller): I think that Buffer.from(data) copies data, so we shouldn't have exceptions with the node
        // addon wiping the data out from under us.
        let buffer = Buffer.from(data)
        let rawOptions = {
          raw: {
            width: width,
            height: height,
            channels: 3
          }
        }
        sharp(buffer, rawOptions)
          .rotate()
          .jpeg()
          .toBuffer((err, data, info) => {
            if (err) {
              return reject(err)
            }
            data = Uint8Array.from(data)
            resolve({data, info})
          })

      })
    })
  },

  extractThumb: function (path) {
    return new Promise(function (resolve, reject) {
      raw.extractThumb(path, function (err, data, width, height) {
        if (err) {
          return reject(err)
        }
        let buffer = Buffer.from(data)
        let rawOptions = {
          raw: {
            width: width,
            height: height,
            channels: 3
          }
        }
        sharp(buffer, rawOptions)
          .rotate()
          .jpeg()
          .toBuffer((err, data, info) => {
            if (err) {
              return reject(err)
            }
            data = Uint8Array.from(data)
            resolve(data)
          })

      })
    })
  }
}

module.exports = libraw
