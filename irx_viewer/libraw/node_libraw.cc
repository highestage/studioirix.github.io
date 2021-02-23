#include <stdio.h>
#include <iostream>
#include <fstream>
#include <v8.h>
#include <nan.h>
#include <codecvt>
#include "libraw/libraw.h"

namespace node_libraw {
  using v8::Exception;
  using v8::Function;
  using v8::FunctionCallbackInfo;
  using v8::FunctionTemplate;
  using v8::Isolate;
  using v8::Local;
  using v8::Number;
  using v8::Object;
  using v8::String;
  using v8::Value;
  using v8::Null;

  std::wstring convertToWstring(std::string utf8String) {

     std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
     return convert.from_bytes( utf8String );
  }

  void Extract(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    v8::String::Utf8Value filenameFromArgs(args[0]->ToString());
    std::string filename = std::string(*filenameFromArgs);
    std::wstring utf16Filename = convertToWstring(filename);

    Local<Function> cb = Local<Function>::Cast(args[1]);

    LibRaw RawProcessor;

    std::ifstream file;
    file.open(utf16Filename, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);

    if (file.read(buffer.data(), size)) {
      RawProcessor.imgdata.params.output_tiff = 0;
      RawProcessor.imgdata.params.output_bps = 8;
      RawProcessor.imgdata.params.use_camera_wb = 1;
      RawProcessor.imgdata.params.user_qual = 0;

      RawProcessor.open_buffer(buffer.data(), size);
      RawProcessor.unpack();
      RawProcessor.dcraw_process();

      libraw_processed_image_t *image = RawProcessor.dcraw_make_mem_image();

      const unsigned argc = 4;
      Local<Value> argv[argc] = {
        Nan::Null(),
        Nan::CopyBuffer(reinterpret_cast<const char*>(image->data), image->data_size).ToLocalChecked(),
        Nan::New<Number>(image->width),
        Nan::New<Number>(image->height),
      };
      cb->Call(Null(isolate), argc, argv);

      LibRaw::dcraw_clear_mem(image);
      RawProcessor.recycle();
    }

    file.close();
  }


  void ExtractThumb(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    v8::String::Utf8Value filenameFromArgs(args[0]->ToString());
    std::string filename = std::string(*filenameFromArgs);

    Local<Function> cb = Local<Function>::Cast(args[1]);

    LibRaw RawProcessor;

    std::ifstream file;
    file.open(filename.c_str(), std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);

    if (file.read(buffer.data(), size)) {
      RawProcessor.imgdata.params.output_tiff = 0;
      RawProcessor.imgdata.params.output_bps = 8;
      RawProcessor.imgdata.params.use_camera_wb = 1;
      RawProcessor.imgdata.params.user_qual = 0;

      RawProcessor.open_buffer(buffer.data(), size);
      RawProcessor.unpack();
      RawProcessor.unpack_thumb();

      const unsigned argc = 2;
      Local<Value> argv[argc] = {
        Nan::Null(),
        Nan::NewBuffer(RawProcessor.imgdata.thumbnail.thumb, RawProcessor.imgdata.thumbnail.tlength).ToLocalChecked()
      };

      cb->Call(Null(isolate), argc, argv);

      RawProcessor.recycle();
    }

    file.close();
  }

  void init(Local<Object> exports) {
    NODE_SET_METHOD(exports, "extract", Extract);
    NODE_SET_METHOD(exports, "extractThumb", ExtractThumb);
  }

  NODE_MODULE(NODE_GYP_MODULE_NAME, init)

}
