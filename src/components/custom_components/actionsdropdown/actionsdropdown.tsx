import { useState } from "react";

type Props = {
   id: string | number;
  onEdit?: (id: string | number) => void;
  onDelete?: (id: string | number) => void;
};

export default function ActionsDropdown({ id, onEdit, onDelete }: Props) {
  const [open, setOpen] = useState(false);

  return (
    <span className="tablehideselect" style={{ position: "relative" }}>
      <div style={{color:"#000"}} className="selecttxtcontainer">
        Actions{" "}
        <span
          style={{ cursor: "pointer" }}
          onClick={() => setOpen((prev) => !prev)}
        >
          <svg
            xmlns="http://www.w3.org/2000/svg"
            width="12"
            height="6"
            viewBox="0 0 12 6"
            fill="none"
          >
            <path
              d="M5.9996 5.90231C5.3971 5.90231 4.8316 5.66791 4.4063 5.24121L0.552798 1.37691C0.260298 1.08391 0.261298 0.609307 0.554298 0.316407C0.848198 0.024407 1.3233 0.0244068 1.6153 0.318407L5.4683 4.18271C5.752 4.46591 6.2476 4.46591 6.5303 4.18271L10.3838 0.318407C10.6758 0.0245068 11.1509 0.024507 11.4448 0.316407C11.7378 0.609407 11.7387 1.08401 11.4463 1.37691L7.5928 5.24121C7.1675 5.66801 6.6017 5.90231 5.9996 5.90231Z"
              fill="#000"
            />
          </svg>
        </span>
      </div>

      {open && (
        <div
          style={{
            position: "absolute",
            top: "100%",
            left:52,
            background: "white",
            boxShadow: "0 2px 6px rgba(0,0,0,0.15)",
            borderRadius: "6px",
            zIndex: 10,
            width: "120px",
            padding: "4px 0",
          }}
        >
          <div className="dropdown-item" onClick={() => onEdit?.(1)}>
            Edit
          </div>
          {/* <div className="dropdown-item" onClick={() => onDelete?.(2)}>
            Delete
          </div> */}
        </div>
      )}
    </span>
  );
}
