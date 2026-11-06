#include "scheduler.h"

// Định nghĩa số lượng task tối đa hệ thống có thể chứa
#define SCH_MAX_TASKS 40

// Khai báo mảng chứa các task và biến đếm số lượng task hiện tại
sTask SCH_tasks_G[SCH_MAX_TASKS];
uint8_t SCH_count = 0;
static uint32_t current_task_id = 0; // Biến cấp phát ID tự động cho task

/* --------------------------------------------------------
 * 1. Hàm khởi tạo bộ lập lịch
 * -------------------------------------------------------- */
void SCH_Init(void) {
    SCH_count = 0;
    current_task_id = 0;

    // Xóa rác trong mảng để đảm bảo an toàn bộ nhớ
    for (int i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_tasks_G[i].pTask = 0;
        SCH_tasks_G[i].Delay = 0;
        SCH_tasks_G[i].Period = 0;
        SCH_tasks_G[i].RunMe = 0;
        SCH_tasks_G[i].TaskID = 0;
    }
}

/* --------------------------------------------------------
 * 2. Hàm cập nhật định kỳ (gọi trong ngắt Timer mỗi 10ms)
 * -------------------------------------------------------- */
void SCH_Update(void) {
    // Nếu không có task nào, thoát ngay lập tức
    if (SCH_count == 0) return;

    // CHỈ xử lý task ở đầu mảng (vị trí 0) - Đảm bảo độ phức tạp O(1)
    if (SCH_tasks_G[0].Delay > 0) {
        SCH_tasks_G[0].Delay--;
    }

    if (SCH_tasks_G[0].Delay == 0) {
        SCH_tasks_G[0].RunMe = 1;
    }
}

/* --------------------------------------------------------
 * 3. Hàm thêm task vào hàng đợi (Core Logic)
 * -------------------------------------------------------- */
uint32_t SCH_Add_Task(void (*pFunc)(void), uint32_t DELAY, uint32_t PERIOD) {
    // 1. Kiểm tra tràn mảng
    if (SCH_count >= SCH_MAX_TASKS) {
        return 0; // Trả về 0 (hoặc mã lỗi) nếu mảng đã đầy
    }

    uint32_t newDelay = DELAY;
    uint8_t index = 0;

    // 2. Tìm vị trí chèn và tính toán Delay tương đối
    for (index = 0; index < SCH_count; index++) {
        if (newDelay > SCH_tasks_G[index].Delay) {
            // Nếu thời gian chạy của task mới lâu hơn task hiện tại,
            // trừ đi thời gian của task hiện tại và tiếp tục duyệt.
            newDelay -= SCH_tasks_G[index].Delay;
        } else {
            // Nếu thời gian chạy của task mới nhỏ hơn hoặc bằng,
            // đây chính là vị trí cần chèn.
            break;
        }
    }

    // 3. Dịch chuyển các task từ vị trí index xuống 1 ô để chừa chỗ
    for (int i = SCH_count; i > index; i--) {
        SCH_tasks_G[i] = SCH_tasks_G[i - 1];
    }

    // 4. Chèn task mới vào vị trí index
    SCH_tasks_G[index].pTask = pFunc;
    SCH_tasks_G[index].Delay = newDelay;
    SCH_tasks_G[index].Period = PERIOD;
    SCH_tasks_G[index].RunMe = 0;

    current_task_id++;
    SCH_tasks_G[index].TaskID = current_task_id;

    // 5. Cập nhật lại Delay của task bị đẩy lùi về sau (task tại index + 1)
    if (index + 1 <= SCH_count) { // Nếu có task đứng sau task vừa chèn
        SCH_tasks_G[index + 1].Delay -= newDelay;
    }

    SCH_count++;
    return SCH_tasks_G[index].TaskID;
}

/* --------------------------------------------------------
 * 4. Hàm xóa task khỏi hàng đợi
 * -------------------------------------------------------- */
uint8_t SCH_Delete_Task(uint32_t taskID) {
    uint8_t index;
    uint8_t found = 0;

    // Tìm vị trí của task dựa trên TaskID
    for (index = 0; index < SCH_count; index++) {
        if (SCH_tasks_G[index].TaskID == taskID) {
            found = 1;
            break;
        }
    }

    if (found == 0) return 0; // Trả về 0 nếu không tìm thấy task

    // QUAN TRỌNG: Nếu task bị xóa không phải là task ở cuối cùng,
    // phải cộng dồn Delay của nó cho task đứng liền sau để trục thời gian không bị gãy.
    if (index + 1 < SCH_count) {
        SCH_tasks_G[index + 1].Delay += SCH_tasks_G[index].Delay;[cite: 12]
    }

    // Dịch chuyển các task phía sau lên trước 1 ô để lấp chỗ trống
    for (int i = index; i < SCH_count - 1; i++) {
        SCH_tasks_G[i] = SCH_tasks_G[i + 1];
    }

    // Xóa sạch dữ liệu ở ô cuối cùng
    SCH_tasks_G[SCH_count - 1].pTask = 0;
    SCH_tasks_G[SCH_count - 1].Delay = 0;
    SCH_tasks_G[SCH_count - 1].Period = 0;
    SCH_tasks_G[SCH_count - 1].RunMe = 0;
    SCH_tasks_G[SCH_count - 1].TaskID = 0;

    SCH_count--;
    return 1; // Xóa thành công
}

/* --------------------------------------------------------
 * 5. Hàm thực thi task (được gọi liên tục trong while(1))
 * -------------------------------------------------------- */
void SCH_Dispatch_Tasks(void) {
    // Chỉ kiểm tra task ở đầu mảng (index 0)
    if (SCH_count > 0 && SCH_tasks_G[0].RunMe > 0) {
        // Lưu tạm thông tin của task trước khi thực thi và xóa
        void (*pFunction)(void) = SCH_tasks_G[0].pTask;
        uint32_t period = SCH_tasks_G[0].Period;
        uint32_t id = SCH_tasks_G[0].TaskID;

        // 1. Thực thi hàm chức năng của task
        (*pFunction)();

        // 2. Xóa task ra khỏi phần đầu của danh sách
        SCH_Delete_Task(id);[cite: 12]

        // 3. Nếu đây là task định kỳ (Period > 0), nạp lại nó vào hệ thống
        // Lưu ý: Task chạy 1 lần (one-shot, Period = 0) sẽ không được add lại
        if (period > 0) {
            SCH_Add_Task(pFunction, period, period);[cite: 12]
        }
    }
}
