class KfsLang < Formula
  desc "Project kfs-lang is simple scripting engine"
  homepage "https://github.com/k0fis/kfs-lang-c"
  license "MIT"

  head do
    url "https://github.com/k0fis/kfs-lang-c", branch: "main"

    depends_on "cmake" => :build
  end

  def install
    system "cmake", "-S", ".", "-B", "build", *std_cmake_args
    system "cmake", "--build", "build"
    system "cmake", "--install", "build"
  end

  test do
    system "false"
  end
end
