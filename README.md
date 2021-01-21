# NXP-EVK

### Table of Contents

* [Hello AI World](#hello-ai-world)
* [Video Walkthroughs](#video-walkthroughs)
* [API Reference](#api-reference)
* [Code Examples](#code-examples)
* [Pre-Trained Models](#pre-trained-models)
* [System Requirements](#recommended-system-requirements)
* [Change Log](CHANGELOG.md)

> &gt; &nbsp; [Jetson Nano 2GB](https://developer.nvidia.com/embedded/jetson-nano-2gb-developer-kit) and JetPack 4.4.1 is now supported in the repo. <br/>
> &gt; &nbsp; Try the new [Re-training SSD-Mobilenet](docs/pytorch-ssd.md) object detection tutorial! <br/>
> &gt; &nbsp; See the [Change Log](CHANGELOG.md) for the latest updates and new features. <br/>

## Hello AI World

Hello AI World can be run completely onboard your Jetson, including inferencing with TensorRT and transfer learning with PyTorch.  The inference portion of Hello AI World - which includes coding your own image classification and object detection applications for Python or C++, and live camera demos - can be run on your Jetson in roughly two hours or less, while transfer learning is best left to leave running overnight.

#### System Setup

* [Setting up Jetson with JetPack](docs/jetpack-setup-2.md)
* [Running the Docker Container](docs/aux-docker.md)
* [Building the Project from Source](docs/building-repo-2.md)

#### Inference

* [Classifying Images with ImageNet](docs/imagenet-console-2.md)
	* [Using the ImageNet Program on Jetson](docs/imagenet-console-2.md)
	* [Coding Your Own Image Recognition Program (Python)](docs/imagenet-example-python-2.md)
	* [Coding Your Own Image Recognition Program (C++)](docs/imagenet-example-2.md)
	* [Running the Live Camera Recognition Demo](docs/imagenet-camera-2.md)
* [Locating Objects with DetectNet](docs/detectnet-console-2.md)
	* [Detecting Objects from Images](docs/detectnet-console-2.md#detecting-objects-from-the-command-line)
	* [Running the Live Camera Detection Demo](docs/detectnet-camera-2.md)
	* [Coding Your Own Object Detection Program](docs/detectnet-example-2.md)
* [Semantic Segmentation with SegNet](docs/segnet-console-2.md)
	* [Segmenting Images from the Command Line](docs/segnet-console-2.md#segmenting-images-from-the-command-line)
	* [Running the Live Camera Segmentation Demo](docs/segnet-camera-2.md)

#### Training

* [Transfer Learning with PyTorch](docs/pytorch-transfer-learning.md)
* Classification/Recognition (ResNet-18)
	* [Re-training on the Cat/Dog Dataset](docs/pytorch-cat-dog.md)
	* [Re-training on the PlantCLEF Dataset](docs/pytorch-plants.md)
	* [Collecting your own Classification Datasets](docs/pytorch-collect.md)
* Object Detection (SSD-Mobilenet)
	* [Re-training SSD-Mobilenet](docs/pytorch-ssd.md)
	* [Collecting your own Detection Datasets](docs/pytorch-collect-detection.md)

#### Appendix

* [Camera Streaming and Multimedia](docs/aux-streaming.md)
* [Image Manipulation with CUDA](docs/aux-image.md)
* [Deep Learning Nodes for ROS/ROS2](https://github.com/dusty-nv/ros_deep_learning)

## Video Walkthroughs

Below are screencasts of Hello AI World that were recorded for the [Jetson AI Certification](https://developer.nvidia.com/embedded/learn/jetson-ai-certification-programs) course:
