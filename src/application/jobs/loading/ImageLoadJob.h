#pragma once

#include <DefHeader.h>
#include <job/system/Job.h>

#include <common/Image/WICMeta.h>
#include <common/Image/WICImageProcessor.h>
#include <engine/resources/copyProviders/GpuUploadTexturePool.h>

namespace app {
	JOB_CLASS(ImageLoadJob) {
		public:
			/// <summary>
			/// Default constructor
			/// </summary>
			ImageLoadJob() = default;

			/// <summary>
			/// New object construtor
			/// </summary>
			/// <param name="refMeta">Image to load</param>
			/// <param name="refPool">Upload pool</param>
			/// <param name="ptrResource">Target resource</param>
			ImageLoadJob(common::image::WIC_META& refMeta, engine::GpuUploadTexturePool& refPool, ID3D12Resource* ptrResource) noexcept;

			/// <summary>
			/// Copy constructor
			/// </summary>
			/// <param name="other">Other job</param>
			ImageLoadJob(const ImageLoadJob& other) noexcept;

			/// <summary>
			/// Assign operator
			/// </summary>
			/// <param name="other">Other job</param>
			/// <returns>Ref to this</returns>
			ImageLoadJob& operator=(const ImageLoadJob & other) noexcept;

			/// <summary>
			/// Job prepare function
			/// </summary>
			/// <param name="args">Arguments</param>
			/// <returns>Prepare descriptor</returns>
			JOB_PREPARE_FUNCTION(void* args);

			/// <summary>
			/// Job execution function
			/// </summary>
			/// <param name="index">Invocation index</param>
			/// <returns>Job result</returns>
			JOB_EXECUTE_FUNCTION(unsigned int index);
		private:
			/// <summary>
			/// Pointer to job meta
			/// </summary>
			common::image::WIC_META* m_ptrMeta = nullptr;

			/// <summary>
			/// Pointer to upload pool
			/// </summary>
			engine::GpuUploadTexturePool* m_ptrPool = nullptr;

			/// <summary>
			/// Pointer to target resource
			/// </summary>
			ID3D12Resource* m_ptrResource = nullptr;
	};
}